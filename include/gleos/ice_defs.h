/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#pragma once

#include "uart.h"

#define ICE_PROTO_VERSION 5
#define ICE_PACKET_DATA_LEN 8

namespace gleos
{
    namespace ice
    {
        using magic_type = uint16_t;
        using address_type = uint16_t;
        using checksum_type = uint16_t;

        static_assert(ICE_PACKET_DATA_LEN % 2 == 0);

        /**
         * Address family.
         * 
         * Some addresses are reserved. They should not be assigned
         * to local devices.
         */
        enum address_family
        {
            broadcast = std::numeric_limits<address_type>::max(),
        };

        /**
         * Message payload type.
         * 
         * This type value indicates which payload is embedded
         * in this message. Some payloads have their own payload
         * structure while others use a generic data type.
         */
        enum payload : uint8_t
        {
            /* Device information */
            device_info_type = 0x10,
            /* Solenoid control */
            solenoid_control_type = 0x11,
            /* Temperature type */
            measurement_temperature_type = 0x12,
            /* Acceleration type */
            measurement_acceleration_type = 0x13,
            /* Angular velocity type */
            measurement_angular_velocity_type = 0x14,
            /* Direction type */
            measurement_direction_type = 0x15,
        };

        enum device_status : uint8_t
        {
            none = 0x0,
            temperature_high = 0x1,
            temperature_critical = 0x2,
        };

        struct __attribute__((packed)) packet
        {
            constexpr static size_t offset = sizeof(magic_type) + sizeof(address_type);

            static_assert(offset % 2 == 0);

            uint8_t version;
            payload payload_type;
        };

        struct __attribute__((packed)) device_info
        {
            constexpr static size_t offset = packet::offset + sizeof(packet);

            address_type address;
            uint8_t version;
            device_status status; // FUTURE: Maybe replace this with u32 uptime.
        };

        // Payload should never exceed ICE_PACKET_DATA_LEN.
        static_assert(sizeof(device_info) <= ICE_PACKET_DATA_LEN);

        // FUTURE: Maybe rename this?
        struct __attribute__((packed)) solenoid_control
        {
            constexpr static size_t offset = packet::offset + sizeof(packet);

            uint8_t id;
            int16_t value;

            inline bool is_halt() const noexcept
            {
                return id == std::numeric_limits<uint8_t>::max() && value == 0;
            }
        };

        // Payload should never exceed ICE_PACKET_DATA_LEN.
        static_assert(sizeof(solenoid_control) <= ICE_PACKET_DATA_LEN);

        /**
         * Scalar with 16-bit value.
         * 
         * Do not take any dependencies on this type. Its size
         * and thus offset can vary.
         */
        struct __attribute__((packed)) scalar16
        {
            constexpr static size_t offset = packet::offset + sizeof(packet);

            int16_t value;
        };

        // Payload should never exceed ICE_PACKET_DATA_LEN.
        static_assert(sizeof(scalar16) <= ICE_PACKET_DATA_LEN);

        /**
         * Vector with 3 dimensions and 16-bit axis.
         * 
         * Do not take any dependencies on this type. Its size
         * and thus offset can vary.
         */
        struct __attribute__((packed)) vector3x16
        {
            constexpr static size_t offset = packet::offset + sizeof(packet);

            int16_t x;
            int16_t y;
            int16_t z;
        };

        // Payload should never exceed ICE_PACKET_DATA_LEN.
        static_assert(sizeof(vector3x16) <= ICE_PACKET_DATA_LEN);

        constexpr size_t frame_size = packet::offset + ICE_PACKET_DATA_LEN + sizeof(checksum_type);

        static_assert(frame_size % 2 == 0);
    }
}
