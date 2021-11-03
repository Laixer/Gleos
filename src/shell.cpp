/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/shell.h"
#include "gleos/status.h"

#include "hardware/vreg.h"

#include <sstream>

using namespace gleos;

shell::shell(uart &device)
    : m_device{device}
{
    status::init();

    std::stringstream ss;

    ss << "\r\nGlonax Embedded Operating System\r\n"
       << "Firmware version : " << GLEOS_FIRMWARE_VERSION_MAJOR << "." << GLEOS_FIRMWARE_VERSION_MINOR << "\r\n"
       << "\r\n"
       << "Type 'help' to see available commands\r\n"
       << "\r\n";

    m_device << ss.str();
}

std::string shell::read_command()
{
    std::string command_buffer;

    char c;
    while ((c = m_device.read_byte()) != '\r')
    {
        if (std::isgraph(c))
        {
            command_buffer += c;
            m_device.write_putc(c);
        }
        else if (c == '\177')
        {
            command_buffer.pop_back();

            // NOTE: Backspace terminal control is always tricky. Depending on
            //       the OS and the terminal settings this may not work as
            //       expected. There is no portable solution.
            m_device << "\b \b";
        }
    }

    return command_buffer;
}

void shell::read()
{
    m_device << "gleos $ ";

    std::string command_buffer = read_command();

    m_device << "\r\n";

    if (command_buffer.empty())
    {
        return;
    }
    else if (command_buffer == "help")
    {
        std::stringstream ss;

        ss << "Commands:\r\n"
           << " help                Show this help message\r\n"
           << " uptime              System uptime in seconds\r\n"
           << " id                  Unique device ID\r\n"
           << " status led [on|off] Toggle status LED\r\n"
           << " reboot              System soft reboot\r\n"
           << " bootsel             Boot into BOOTSEL mode\r\n"
           << " version             Firmware version\r\n";

        m_device << ss.str();
    }
    else if (command_buffer == "status led on")
    {
        status::status_led(true);
    }
    else if (command_buffer == "status led off")
    {
        status::status_led(false);
    }
    else if (command_buffer == "uptime")
    {
        std::stringstream ss;

        ss << " up "
           << sec_since_boot()
           << " seconds \r\n";

        m_device << ss.str();
    }
    else if (command_buffer == "id")
    {
        std::stringstream ss;

        ss << unique_id()
           << "\r\n";

        m_device << ss.str();
    }
    else if (command_buffer == "reboot")
    {
        reboot();
    }
    else if (command_buffer == "bootsel")
    {
        reboot(boot_mode::bootsel);
    }
    else if (command_buffer == "boost")
    {
        vreg_set_voltage(VREG_VOLTAGE_MAX);
    }
    else if (command_buffer == "version")
    {
        std::stringstream ss;

        ss << "Firmware version " << GLEOS_FIRMWARE_VERSION_MAJOR << "." << GLEOS_FIRMWARE_VERSION_MINOR << "\r\n";

        m_device << ss.str();
    }
    else
    {
        std::stringstream ss;

        ss << command_buffer << ": command not found\r\n";

        m_device << ss.str();
    }
}
