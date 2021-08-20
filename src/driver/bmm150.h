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

class bmm150 : public gleos::i2c::driver
{
    /* Sensor settings. */
    struct bmm150_settings
    {
        /* Control measurement of XYZ axes. */
        uint8_t xyz_axes_control;
        /* Power control bit value. */
        uint8_t pwr_cntrl_bit;
        /* Power control bit value. */
        uint8_t pwr_mode;
        /* Data rate value (ODR). */
        uint8_t data_rate;
        /* XY Repetitions. */
        uint8_t xy_rep;
        /* Z Repetitions. */
        uint8_t z_rep;
        /* Preset mode of sensor. */
        uint8_t preset_mode;
    };

    void set_odr_xyz_rep(struct bmm150_settings settings);

    void set_preset_mode(uint8_t preset_mode);

    void write_op_mode(uint8_t op_mode);
    void set_power_control_bit(uint8_t pwrcntrl_bit);

    void set_power_mode(uint8_t pwr_mode);

public:
    bmm150(gleos::i2c::block &block);

    virtual bool driver_is_alive() override;
    virtual void driver_reset() override;

    void read_mag_vector3(int16_t &x, int16_t &y, int16_t &z);
};
