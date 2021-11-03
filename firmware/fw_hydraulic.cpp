/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

// TODO:
// - Internal temperature via ADC

#include "gleos/motor.h"
#include "gleos/watchdog.h"
#include "gleos/layer3.h"
#include "gleos/interval.h"

#include <iostream>

#define UART_ID uart1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define ICE_DEVICE_ADDR 0x7
#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 3

enum actuator_designation
{
    slew_high = 12,
    slew_low = 13,
    boom_high = 10,
    boom_low = 11,
    bucket_high = 6,
    bucket_low = 7,
    arm_high = 8,
    arm_low = 9,
    crawl_left_high = 20,
    crawl_left_low = 21,
    crawl_right_high = 18,
    crawl_right_low = 19,
};

int main()
{
    // Enable console.
    gleos::stdio_console_port();

    // Set the watch deadtime to 2s.
    // gleos::watchdog::start(2000);

    // Initialize all dual motor actuators.
    std::array<gleos::actuator::motor, 6> motor_pwm{
        // Index: 0; Actuate: Bucket
        gleos::actuator::motor{bucket_high, bucket_low},
        // Index: 1; Actuate: Arm
        gleos::actuator::motor{arm_high, arm_low},
        // Index: 2; Actuate: Boom
        gleos::actuator::motor{boom_high, boom_low},
        // Index: 3; Actuate: Slew
        gleos::actuator::motor{slew_high, slew_low},
        // Index: 4; Actuate: Crawl left
        gleos::actuator::motor{crawl_left_high, crawl_left_low},
        // Index: 5; Actuate: Crawl right
        gleos::actuator::motor{crawl_right_high, crawl_right_low},
    };

    for (auto &pwm : motor_pwm)
    {
        pwm.enable();
    }

    // Open the data channel.
    gleos::uart serial{UART_ID, UART_TX_PIN, UART_RX_PIN};
    gleos::ice::layer3 netlayer{serial, ICE_DEVICE_ADDR, {FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR}};

    const auto periodic_update = [&]
    {
        // Announce this device on the network.
        netlayer.announce_device();

        // Postpone deadline timer.
        // gleos::watchdog::update();

        std::cout << "Announce device on network" << std::endl;
        std::cout << "Uptime: " << gleos::sec_since_boot() << " seconds" << std::endl;

        return true;
    };

    gleos::timer_interval timer{gleos::ice::broadcast_service::default_interval, periodic_update};

    // Postpone deadline timer.
    // gleos::watchdog::update();

    while (true)
    {
        auto frame = netlayer.accept();

        // gleos::watchdog::update();

        switch (frame.get<gleos::ice::packet>()->payload_type)
        {

        case gleos::ice::payload::device_info_type:
        {
            const auto dev_info = frame.get<gleos::ice::device_info>();

            std::cout << "Device announcement" << '\n'
                      << " Address: " << dev_info->address << '\n'
                      << " Version: " << (dev_info->version >> 4) << "." << static_cast<int>(dev_info->version & ~0xf0) << '\n'
                      << " Status: " << dev_info->status << std::endl;
            break;
        }

        case gleos::ice::payload::solenoid_control_type:
        {
            const auto solenoid_ctrl = frame.get<gleos::ice::solenoid_control>();

            std::cout << "Request for valve control" << std::endl;

            // In the exceptional case that halt is requested we
            // instructed all motors to write an explicit 0 on both
            // sides of the actuator.
            if (solenoid_ctrl->is_halt())
            {
                for (auto &pwm : motor_pwm)
                {
                    pwm.set_all(0);
                }

                std::cout << "Halt all actuators" << std::endl;
            }
            else if (solenoid_ctrl->id <= motor_pwm.size() - 1)
            {
                std::cout << "Move valve " << static_cast<int>(solenoid_ctrl->id) << " to value " << solenoid_ctrl->value << std::endl;

                motor_pwm[solenoid_ctrl->id].set_motion_value(solenoid_ctrl->value);
            }
            else
            {
                std::cout << "Invalid solenoid id" << std::endl;
            }

            break;
        }

        default:
            std::cout << "Invalid payload type" << std::endl;
            break;
        }
    }

    return 0;
}
