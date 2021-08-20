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

#include "pico/time.h"

namespace gleos
{
    /**
     * Run method on timer interval.
     * 
     * This class can also be extended.
     */
    class timer_interval
    {
        using callback_type = std::function<void(void)>;

        struct repeating_timer m_timer;
        callback_type m_callback;

        /**
         * This method is invoked on timer trigger.
         */
        virtual void invoke() const
        {
        }

        /**
         * This callback is invoked on timer trigger.
         */
        static bool timer_interval_callback(struct repeating_timer *timer)
        {
            const auto this_timer = reinterpret_cast<const timer_interval *>(timer->user_data);
            if (this_timer->m_callback)
            {
                this_timer->m_callback();
            }
            this_timer->invoke();
            return true;
        }

    public:
        /**
         * Run method on timer interval.
         */
        timer_interval(uint32_t delay_ms, callback_type callback = nullptr)
            : m_callback{callback}
        {
            if (!add_repeating_timer_ms(delay_ms, timer_interval_callback, this, &m_timer))
            {
                // TODO: ...
                printf("Failed to add timer\n");
            }
        }
    };
}
