/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#pragma once

#include "gleos.h"

#include "hardware/flash.h"

/* Flash file offset start */
#define FLASH_FILE_OFFSET (1024 * 1024)
/* Flash file magic value */
#define FLASH_FILE_MAGIC 0x7a0c

namespace gleos::flash
{
    namespace detail
    {
        template <typename T>
        struct flash_file
        {
            uint16_t magic;
            T file;
        };

        template <typename T>
        inline constexpr const uint8_t *flash_file_location()
        {
            return reinterpret_cast<const uint8_t *>(XIP_BASE + FLASH_FILE_OFFSET + T::offset);
        }
    }

    template <typename T>
    inline constexpr bool has_file()
    {
        using namespace detail;

        const auto file = reinterpret_cast<const flash_file<T> *>(flash_file_location<T>());
        return file->magic == FLASH_FILE_MAGIC;
    }

    template <typename T>
    inline constexpr T open_file()
    {
        using namespace detail;

        const auto file = reinterpret_cast<const flash_file<T> *>(flash_file_location<T>());
        return file->file;
    }

    template <typename T>
    bool save_file(const T &in_file)
    {
        using namespace detail;

        uint8_t buffer[FLASH_PAGE_SIZE];

        flash_file<T> file{
            magic : FLASH_FILE_MAGIC,
            file : in_file,
        };

        if (sizeof(file) < FLASH_PAGE_SIZE)
        {
            memcpy(buffer, &file, sizeof(file));

            flash_range_erase(FLASH_FILE_OFFSET + T::offset, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_FILE_OFFSET + T::offset, buffer, FLASH_PAGE_SIZE);

            return true;
        }

        return false;
    }
} // gleos
