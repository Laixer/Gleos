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

#include "gleos.h"
#include "pwm.h"

namespace gleos
{
    namespace actuator
    {
        class motor : public pulse_modulation
        {
        public:
            motor(int port_a, int port_b);

            void set_motion_value(int64_t value) noexcept;
        };
    }
}