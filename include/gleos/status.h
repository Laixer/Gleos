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

namespace gleos::status
{
    /**
     * Initialize status pheripherials.
     */
    void init();

    /**
     * Enable/disable status LED.
     * 
     * @param is_high Trigger LED high or low.
     */
    void status_led(bool is_high);
} // gleos
