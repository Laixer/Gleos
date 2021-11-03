/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/watchdog.h"

#include "hardware/watchdog.h"

using namespace gleos;

void watchdog::start(uint32_t delay_ms)
{
    watchdog_enable(delay_ms, 1);
}

void watchdog::update()
{
    watchdog_update();
}

bool watchdog::caused_reboot()
{
    return watchdog_caused_reboot();
}

void watchdog::reboot()
{
    watchdog_reboot(0, SRAM_END, 0);
}
