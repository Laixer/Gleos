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

#include "config.h"

#include <cstddef>
#include <cstdint>

namespace gleos
{
    /**
     * Sleep the current thread.
     * 
     * @param delay_ms Miliseconds to sleep.
     */
    void sleep(uint32_t delay_ms) noexcept;

    /**
     * Return the time since boot in seconds.
     */
    uint32_t sec_since_boot() noexcept;

    /**
     * Return the time since boot in miliseconds.
     */
    uint32_t ms_since_boot() noexcept;

    /**
     * Get unique hardware identifier.
     */
    long unique_id() noexcept;

    /**
     * Redirect all standard IO to and from console port.
     * 
     * It is recommended to only enable this for debug builds.
     */
    void stdio_console_port() noexcept;

    /**
     * Calculate CRC16/IBM_3740 over buffer.
     * 
     * @param data      Pointer to buffer data.
     * @param length    Buffer length.
     */
    uint16_t crc16(const uint8_t *data, size_t length);

    /**
     * Convert buffer to signed 16-bit little endian.
     * 
     * NOTE: This method does not do bounds checks. It
     *       is up to the caller to ensure buffer is of
     *       a sufficient size.
     * 
     * @param buffer    Input buffer of type T.
     * @return          Signed 16-bit little endian.
     */
    template <typename T>
    inline int16_t buffer_to_i16le(T buffer[])
    {
        return buffer[0] | (buffer[1] << 8);
    }

    /**
     * Convert buffer to signed 16-bit.
     * 
     * NOTE: This method does not do bounds checks. It
     *       is up to the caller to ensure buffer is of
     *       a sufficient size.
     * 
     * @param buffer    Input buffer of type T.
     * @return          Signed 16-bit.
     */
    template <typename T>
    inline int16_t buffer_to_i16(T buffer[])
    {
        return (buffer[0] << 8) | buffer[1];
    }
} // gleos
