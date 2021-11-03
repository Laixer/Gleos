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
#include "uart.h"

namespace gleos
{
    class shell
    {
        uart &m_device;

    public:
        shell(uart &device);

        void read();

    private:
        std::string read_command();
    };
} // gleos
