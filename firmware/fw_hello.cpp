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
#include "gleos/status.h"

#include <iostream>
#include <sstream>

int main()
{
    // Enable console.
    gleos::stdio_console_port();

    // Initialize the status device.
    gleos::status::init();

    gleos::uart uart{uart1, 115200};

    while (true)
    {
        gleos::status::status_led(true);

        {
            std::stringstream ss;

            ss << "[UART0] ["
               << gleos::ms_since_boot() / 100
               << "] "
               << "Hello, world!, I am device: "
               << gleos::unique_id()
               << "\n"
               << std::flush;

            std::cout << ss.str();
        }

        {
            std::stringstream ss;

            ss << "[UART1] ["
               << gleos::ms_since_boot() / 100
               << "] "
               << "Hello, world!, I am device: "
               << gleos::unique_id()
               << "\r\n"
               << std::flush;

            uart << ss.str();
        }

        gleos::status::status_led(false);

        gleos::sleep(500);
    }

    return 0;
}
