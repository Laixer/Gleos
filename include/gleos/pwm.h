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

namespace gleos
{
    class pulse_modulation
    {
        unsigned int m_slice;

    public:
        /* Create new pulse modulation instance. */
        pulse_modulation(int port_a, int port_b);

        /* Reset all PWM slices. */
        static void reset() noexcept;

        /* Enable PWM clock on this slice. */
        void enable() noexcept;
        /* Disable PWM clock on this slice. */
        void disable() noexcept;

        /* Set value on a PWM channel. */
        void set_channel(unsigned int channel, uint16_t value = 0) noexcept;
        /* Set value on first two PWM channels. */
        void set_dual_channel(uint16_t value_a = 0, uint16_t value_b = 0) noexcept;
        /* Set value on all PWM channels. */
        void set_all(uint16_t value = 0) noexcept;
    };
}
