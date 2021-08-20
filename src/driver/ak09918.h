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

#include "gleos/i2c.h"

class ak09918 : public gleos::i2c::driver
{
public:
    enum operation_mode
    {
        power_down = 0x00,
        single_measurement = 0x01,
        continuous_10hz = 0x02,
        continuous_20hz = 0x04,
        continuous_50hz = 0x06,
        continuous_100hz = 0x08,
        self_test = 0x10,
    };

public:
    ak09918(gleos::i2c::block &block);

    void set_operation_mode(operation_mode mode);

    /**
     * Check if data is ready to be read.
     * 
     * @return True if data is ready, false otherwise.
     */
    bool is_data_ready();

    /**
     * Check if data was preceded by new data.
     * 
     * @return True if data is overwritten, false otherwise.
     */
    bool is_data_skipped();

    virtual bool driver_is_alive() override;
    virtual void driver_reset() override;
    virtual bool driver_set_power_mode(power_mode mode) override;

    void read_mag_vector3(int16_t &x, int16_t &y, int16_t &z);
};
