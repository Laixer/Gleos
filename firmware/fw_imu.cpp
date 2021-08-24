/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/layer3.h"

#include "driver/icm20600.h"

#include <iostream>

#define UART_ID uart1
#define BAUD_RATE 115200

#define ICE_DEVICE_ADDR 0x9

int main()
{
    // Enable logger console.
    gleos::stdio_console_port();

    gleos::uart serial{UART_ID, BAUD_RATE};

    gleos::ice::layer3 netlayer{serial, ICE_DEVICE_ADDR};

    gleos::i2c::block i2c_0{20, 21, gleos::i2c::mode::fast_mode};

    icm20600 sensor{i2c_0};

    const auto periodic_update = [&]
    {
        // Announce this device on the network.
        netlayer.announce_device();

        // Postpone deadline timer.
        // gleos::watchdog::update();

        std::cout << "Announce device on network" << std::endl;
        std::cout << "Uptime since boot: " << gleos::sec_since_boot() << " seconds" << std::endl;

        return true;
    };

    gleos::timer_interval timer{500, periodic_update};

    while (true)
    {
        if (!sensor.driver_is_alive())
        {
            // TODO: Send this to other end.
            std::cout << "Sensor not ready" << std::endl;

            gleos::sleep(50);

            continue;
        }

        {
            int16_t x, y, z;
            sensor.read_acc_vector3(x, y, z);
            std::cout << "Acc: X: " << x << " Y: " << y << " Z: " << z << std::endl;

            netlayer.dispatch_acceleration(gleos::ice::address_family::broadcast, x, y, z);
        }

        // {
        //     int16_t temp;
        //     sensor.read_temperature(temp);
        //     std::cout << "Temp: " << temp << std::endl;

        //     netlayer.dispatch_temperature(gleos::ice::address_family::broadcast, temp);
        // }

        // gleos::sleep(5);
    }

    return 0;
}
