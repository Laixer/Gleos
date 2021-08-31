/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/layer3.h"

#include <cstring>
#include <iostream>

using namespace gleos::ice;

// TODO: std::array
const uint8_t magic[2] = {0xc5, 0x34};

struct checksum_helper
{
    constexpr static size_t offset = packet::offset + ICE_PACKET_DATA_LEN;

    checksum_type checksum;
};

frame::frame()
{
    // FUTURE: Can this be removed?
    std::memset(m_buffer.data(), '\0', 14);
}

bool frame::is_valid()
{
    if (m_buffer[sizeof(magic[0])] != magic[1])
    {
        std::cout << "Invalid magic" << std::endl;
        return false;
    }

    auto local_crc = crc16(m_buffer.data() + packet::offset, ICE_PACKET_DATA_LEN);
    if (local_crc != get<checksum_helper>()->checksum)
    {
        std::cout << "Invalid checksum" << std::endl;
        return false;
    }

    if (get<packet>()->version != ICE_PROTO_VERSION)
    {
        std::cout << "Invalid version" << std::endl;
        return false;
    }

    return true;
}

void frame::set_address(address_type address)
{
    set<address_type, sizeof(magic_type)>(address);
}

// FUTURE: Create a frame builder.
void frame::build()
{
    m_buffer[0] = magic[0];
    m_buffer[1] = magic[1];

    set(checksum_helper{
        checksum : crc16(m_buffer.data() + packet::offset, ICE_PACKET_DATA_LEN)
    });
}

layer3::layer3(uart &device, address_type address, std::pair<unsigned int, unsigned int> version)
    : m_device{device}, m_address{address}, m_version{version}
{
}

frame layer3::accept()
{
    frame frame;

    while (true)
    {
        // Keep reading until the first magic byte is found.
        // Only thereafter we'll read the entire frame. Any
        // buffer misalignments will be discarded.
        while (m_device.read_byte() != magic[0])
        {
            tight_loop_contents();
        }

        // Read the entire frame at once. This is the most efficient
        // way to buffer data. It has the side effect that any frame
        // is guaranteed to be natural aligned.
        m_device.read(frame.buffer() + sizeof(magic[0]), frame_size - sizeof(magic[0]));

        if (!frame.is_valid())
        {
            continue;
        }

        // Ignore misaddressed packets.
        if (!frame.is_broadcast() && frame.address() != m_address)
        {
            std::cout << "Ignore packet with address: " << frame.address() << std::endl;
            continue;
        }

        return frame;
    }
}

void layer3::announce_device()
{
    frame frame;

    frame.set_address(address_family::broadcast);
    frame.set(packet{
        version : ICE_PROTO_VERSION,
        payload_type : payload::device_info_type,
    });
    frame.set(device_info{
        address : m_address,
        version : static_cast<uint8_t>(m_version.first | (m_version.second << 4)),
        status : device_status::none,
    });
    frame.build();

    m_device.write(frame.buffer(), frame_size);
}

void layer3::dispatch_temperature(address_type address, int16_t temperature)
{
    frame frame;

    frame.set_address(address);
    frame.set(packet{
        version : ICE_PROTO_VERSION,
        payload_type : payload::measurement_temperature_type,
    });
    frame.set(scalar16{
        value : temperature,
    });
    frame.build();

    m_device.write(frame.buffer(), frame_size);
}

void layer3::dispatch_acceleration(address_type address, int16_t x, int16_t y, int16_t z)
{
    frame frame;

    frame.set_address(address);
    frame.set(packet{
        version : ICE_PROTO_VERSION,
        payload_type : payload::measurement_acceleration_type,
    });
    frame.set(vector3x16{
        x,
        y,
        z,
    });
    frame.build();

    m_device.write(frame.buffer(), frame_size);
}

void broadcast_service::invoke() const
{
    m_layer.announce_device();

    std::cout << "Sending device announcement" << std::endl;
    std::cout << "Uptime since boot: " << gleos::ms_since_boot() << std::endl;
}

broadcast_service::broadcast_service(uint32_t delay_ms, layer3 &layer)
    : timer_interval{delay_ms}, m_layer{layer}
{
}
