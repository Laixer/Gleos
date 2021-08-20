/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/uart.h"

#include "pico/time.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <iostream>
#include <sstream>

int main()
{
    gleos::stdio_console_port();

    spi_init(spi_default, 10000000); // 10MHz
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    while (true)
    {
        absolute_time_t boot_time = get_absolute_time();
        auto ten_sec = to_ms_since_boot(boot_time) / 100;

        {
            std::stringstream ss;

            ss << "[UART0] ["
               << ten_sec
               << "] "
               << "Hello, world!, I am device: "
               << gleos::unique_id()
               << "\n"
               << std::flush;

            std::cout << ss.str();
        }

        gleos::sleep(500);
    }

    return 0;
}
