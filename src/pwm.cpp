/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/pwm.h"

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/resets.h"

using namespace gleos;

const uint16_t cycle_max = 255;

pulse_modulation::pulse_modulation(int port_a, int port_b)
{
    gpio_set_function(port_a, GPIO_FUNC_PWM);
    gpio_set_function(port_b, GPIO_FUNC_PWM);

    m_slice = pwm_gpio_to_slice_num(port_a);
    assert(m_slice == pwm_gpio_to_slice_num(port_b));

    pwm_config config = pwm_get_default_config();
    // TODO: It is unclear if we should keep this.
    pwm_config_set_clkdiv(&config, 8.f);
    pwm_config_set_wrap(&config, cycle_max);
    pwm_init(m_slice, &config, false);
}

void pulse_modulation::reset() noexcept
{
    reset_block(RESETS_RESET_PWM_BITS);
    unreset_block_wait(RESETS_RESET_PWM_BITS);
}

void pulse_modulation::enable() noexcept
{
    pwm_set_enabled(m_slice, true);
}

void pulse_modulation::disable() noexcept
{
    pwm_set_enabled(m_slice, false);
}

void pulse_modulation::set_channel(unsigned int channel, uint16_t value) noexcept
{
    switch (channel)
    {
    case PWM_CHAN_A:
        pwm_set_chan_level(m_slice, PWM_CHAN_A, value);
        break;
    case PWM_CHAN_B:
        pwm_set_chan_level(m_slice, PWM_CHAN_B, value);
        break;
    }
}

void pulse_modulation::set_dual_channel(uint16_t value_a, uint16_t value_b) noexcept
{
    pwm_set_both_levels(m_slice, std::min(value_a, cycle_max), std::min(value_b, cycle_max));
}

void pulse_modulation::set_all(uint16_t value) noexcept
{
    set_dual_channel(value, value);
}
