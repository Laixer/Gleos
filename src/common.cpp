/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/gleos.h"
#include "gleos/uart.h"
#include "gleos/watchdog.h"
#include "gleos/flash.h"

#include "pico/stdlib.h"
#include "pico/stdio_uart.h"
#include "pico/unique_id.h"
#include "pico/bootrom.h"

#define GLEOS_STDIO_TX_PIN 0
#define GLEOS_STDIO_RX_PIN 1

namespace gleos
{
    uint16_t device_id = 0x2500;

    namespace detail
    {
        struct device_config
        {
            static const int offset = 0;

            uint16_t device_id;
        };
    }

    void bootstrap()
    {
        using namespace detail;

        // There should be a device configuration file at any time. If the file
        // was not found then is must be created now. The device configuration
        // must survive future firmware updates.
        if (flash::has_file<device_config>())
        {
            auto file = flash::open_file<device_config>();

            device_id = file.device_id;
        }
        else
        {
            device_config file{
                device_id : device_id, // 0x2519
            };

            // TODO: Handle result
            flash::save_file(file);
        }
    }

    void sleep(uint32_t delay_ms) noexcept
    {
        sleep_ms(delay_ms);
    }

    uint32_t sec_since_boot() noexcept
    {
        return to_ms_since_boot(get_absolute_time()) / 1000;
    }

    uint32_t ms_since_boot() noexcept
    {
        return to_ms_since_boot(get_absolute_time());
    }

    void reboot(boot_mode mode) noexcept
    {
        if (mode == boot_mode::bootsel)
        {
            reset_usb_boot(0, 0);
        }
        else
        {
            // Soft reboot is performed via the watchdog reboot action. The
            // watchdog reboot will instruct the CPU to time-out instantly
            // causing the reboot.
            watchdog::reboot();
        }
    }

    long unique_id() noexcept
    {
        pico_unique_board_id_t id;
        pico_get_unique_board_id(&id);

        // NOTE: This cast only works because the datastructure was already
        //       memory aligned by the compiler. If this will ever change
        //       we *could* trigger a hardfault.
        return reinterpret_cast<uint64_t>(id.id);
    }

    void stdio_console_port() noexcept
    {
        // Erase any previously configured settings from memory.
        uart::unset(uart0);

        // NOTE: The console port claims the hardware UART device, which cannnot
        //       be allocated again.
        stdio_uart_init_full(uart0, GLEOS_DEFAULT_UART_BAUD_RATE, GLEOS_STDIO_TX_PIN, GLEOS_STDIO_RX_PIN);
    }

    // FUTURE: CRC should not be calculated in software.
    uint16_t crc16(const uint8_t *data, size_t length)
    {
        uint8_t tmp = 0;
        uint16_t crc = 0xffff;

        while (length--)
        {
            tmp = crc >> 8 ^ *data++;
            tmp ^= tmp >> 4;
            crc = (crc << 8) ^ ((uint16_t)(tmp << 12)) ^ ((uint16_t)(tmp << 5)) ^ ((uint16_t)tmp);
        }

        return crc;
    }

    namespace status
    {
        void init()
        {
            gpio_init(PICO_DEFAULT_LED_PIN);
            gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        }

        void status_led(bool is_high)
        {
            gpio_put(PICO_DEFAULT_LED_PIN, is_high);
        }
    }
} // namespace gleos
