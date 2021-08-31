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
#include "interval.h"
#include "ice_defs.h"

namespace gleos
{
    namespace ice
    {
        class frame
        {
            std::array<uint8_t, frame_size> m_buffer;

        public:
            frame();
            frame(const frame &) = delete;
            frame(frame &&) = default;

            /**
             * Return internal buffer.
             * 
             * Use this function only for raw operations. All
             * access is unguarded.
             */
            inline auto buffer()
            {
                return m_buffer.data();
            }

            /**
             * Test if frame is valid.
             * 
             * @return True if frame is valid, false otherwise.
             */
            bool is_valid();

            /**
             * Set frame address.
             */
            void set_address(address_type address);

            /**
             * Build a valid frame.
             * 
             * This will set the magic value and checksum. Frame *must* be
             * valid after build is called.
             */
            void build();

            /**
             * Access object in buffer.
             */
            template <typename T, size_t N = T::offset>
            inline const T *get() const noexcept
            {
                return reinterpret_cast<const T *>(m_buffer.data() + N);
            }

            /**
             * Set object in buffer.
             */
            template <typename T, size_t N = T::offset>
            inline void set(T object) noexcept
            {
                (*reinterpret_cast<T *>(m_buffer.data() + N)) = object;
            }

            /**
             * Get frame address.
             * 
             * @return Address enlisted in the frame.
             */
            inline auto address() const noexcept
            {
                return *get<address_type, sizeof(magic_type)>();
            }

            /**
             * Check if frame is broadcast message.
             * 
             * A broadcast message should be accepted by all devices
             * on the network even if they cannot use the message themselves.
             * 
             * @return True if broadcast frame, false otherwise.
             */
            inline auto is_broadcast() const noexcept
            {
                return address() == address_family::broadcast;
            }
        };

        class layer3
        {
            address_type m_address;
            std::pair<unsigned int, unsigned int> m_version;
            uart &m_device;

        public:
            /**
             * Construct layer instance.
             * 
             * @param device    Device which can read and write data blobs.
             * @param address   Local device address. 
             */
            layer3(uart &device, address_type address, std::pair<unsigned int, unsigned int> version);

            /**
             * Announce this device on the network.
             */
            void announce_device();

            /**
             * Announce temperature on the network.
             */
            void dispatch_temperature(address_type address, int16_t temperature);

            /**
             * Announce vector3 on the network.
             * 
             * @param address   Recipient address.
             * @param x         Vector X axis.
             * @param y         Vector Y axis.
             * @param z         Vector Z axis.
             */
            void dispatch_acceleration(address_type address, int16_t x, int16_t y, int16_t z);

            /**
             * Accept the next application frame.
             * 
             * This method neglects any empty or none usable frames.
             * The returning frame is guaranteed to contain a packet.
             */
            frame accept();
        };

        /**
         * Broadcast service.
         * 
         * Announce this device on the network. There is no need
         * to do this more than once every second. However it is
         * recommended to broadcast at least *once* every second.
         */
        class broadcast_service : public timer_interval
        {
            layer3 &m_layer;

            /**
             * Run the broadcast routine.
             */
            void invoke() const override;

        public:
            /**
             * Construct broadcast service instance.
             * 
             * @param delay_ms  Service timer interval in miliseconds.
             * @param layer     Layer instance. 
             */
            broadcast_service(uint32_t delay_ms, layer3 &layer);
        };
    }
}
