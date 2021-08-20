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
    namespace watchdog
    {
        /**
         * Start the watchdog.
         * 
         * Will reboot if `update` is not called at least once within
         * the delay period. The delay is in miliseconds.
         */
        void start(uint32_t delay_ms);

        /**
         * Update watchdog deadline timer.
         */
        void update();

        /**
         * Was the reboot caused by the watchdog.
         */
        bool caused_reboot();
    };
}
