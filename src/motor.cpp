/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/motor.h"

#include <cmath>

using namespace gleos::actuator;

motor::motor(int port_a, int port_b)
    : pulse_modulation{port_a, port_b}
{
}

void motor::set_motion_value(int64_t value) noexcept
{
    if (value > 0)
    {
        set_dual_channel(value, 0);
    }
    else if (value < 0)
    {
        set_dual_channel(0, std::abs(value));
    }
    else
    {
        set_dual_channel(0, 0);
    }
}
