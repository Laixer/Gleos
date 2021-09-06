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

#include <algorithm>

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
    pwm_init(m_slice, &config, m_is_enabled);

    // Set both channels to inital value.
    pwm_set_both_levels(m_slice, m_chan_a_value, m_chan_b_value);
}

void pulse_modulation::reset() noexcept
{
    reset_block(RESETS_RESET_PWM_BITS);
    unreset_block_wait(RESETS_RESET_PWM_BITS);
}

void pulse_modulation::enable() noexcept
{
    pwm_set_enabled(m_slice, true);
    m_is_enabled = true;
}

void pulse_modulation::disable() noexcept
{
    pwm_set_enabled(m_slice, false);
    m_is_enabled = false;
}

void pulse_modulation::set_channel(unsigned int channel, uint16_t value) noexcept
{
    if (!m_is_enabled)
    {
        return;
    }

    auto value_new = std::min(value, cycle_max);
    switch (channel)
    {

    case PWM_CHAN_A:
        if (value_new != m_chan_a_value)
        {
            pwm_set_chan_level(m_slice, PWM_CHAN_A, value_new);
            m_chan_a_value = value_new;
        }
        break;

    case PWM_CHAN_B:
        if (value_new != m_chan_b_value)
        {
            pwm_set_chan_level(m_slice, PWM_CHAN_B, value_new);
            m_chan_b_value = value_new;
        }
        break;
    }
}

void pulse_modulation::set_dual_channel(uint16_t value_a, uint16_t value_b) noexcept
{
    if (!m_is_enabled)
    {
        return;
    }

    auto value_a_new = std::min(value_a, cycle_max);
    auto value_b_new = std::min(value_b, cycle_max);
    if (value_a_new != m_chan_a_value || value_b_new != m_chan_b_value)
    {
        pwm_set_both_levels(m_slice, value_a_new, value_b_new);

        m_chan_a_value = value_a_new;
        m_chan_b_value = value_b_new;
    }
}

void pulse_modulation::set_all(uint16_t value) noexcept
{
    set_dual_channel(value, value);
}
