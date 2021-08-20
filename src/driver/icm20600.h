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

class icm20600 : public gleos::i2c::driver
{
    uint16_t m_acc_scale{0}, m_gyro_scale{0};

    enum power_mode
    {
        icm_sleep_mode,
        icm_standyby_mode,
        icm_acc_low_power,
        icm_acc_low_noise,
        icm_gyro_low_power,
        icm_gyro_low_noise,
        icm_6axis_low_power,
        icm_6axis_low_noise,
    };

    void set_power_mode(power_mode mode);

    // Accelerometer scale range
    enum acc_scale_type_t
    {
        RANGE_2G = 0,
        RANGE_4G,
        RANGE_8G,
        RANGE_16G,
    };

    void set_acc_scale_range(acc_scale_type_t range);

    // Accelerometer output data rate
    enum acc_lownoise_odr_type_t
    {
        ACC_RATE_4K_BW_1046 = 0,
        ACC_RATE_1K_BW_420,
        ACC_RATE_1K_BW_218,
        ACC_RATE_1K_BW_99,
        ACC_RATE_1K_BW_44,
        ACC_RATE_1K_BW_21,
        ACC_RATE_1K_BW_10,
        ACC_RATE_1K_BW_5,
    };

    void set_acc_output_data_rate(acc_lownoise_odr_type_t odr);

    // Averaging filter settings for Low Power Accelerometer mode
    enum acc_averaging_sample_type_t
    {
        ACC_AVERAGE_4 = 0,
        ACC_AVERAGE_8,
        ACC_AVERAGE_16,
        ACC_AVERAGE_32,
    };

    void set_acc_average_sample(acc_averaging_sample_type_t sample);

    // Gyroscope scale range
    enum gyro_scale_type_t
    {
        RANGE_250_DPS = 0,
        RANGE_500_DPS,
        RANGE_1K_DPS,
        RANGE_2K_DPS,
    };

    void set_gyro_scale_range(gyro_scale_type_t range);

    // Averaging filter configuration for low-power gyroscope mode
    enum gyro_averaging_sample_type_t
    {
        GYRO_AVERAGE_1 = 0,
        GYRO_AVERAGE_2,
        GYRO_AVERAGE_4,
        GYRO_AVERAGE_8,
        GYRO_AVERAGE_16,
        GYRO_AVERAGE_32,
        GYRO_AVERAGE_64,
        GYRO_AVERAGE_128,
    };

    void set_gyro_average_sample(gyro_averaging_sample_type_t sample);

    // Gyroscope output data rate
    enum gyro_lownoise_odr_type_t
    {
        GYRO_RATE_8K_BW_3281 = 0,
        GYRO_RATE_8K_BW_250,
        GYRO_RATE_1K_BW_176,
        GYRO_RATE_1K_BW_92,
        GYRO_RATE_1K_BW_41,
        GYRO_RATE_1K_BW_20,
        GYRO_RATE_1K_BW_10,
        GYRO_RATE_1K_BW_5,
    };

    void set_gyro_output_data_rate(gyro_lownoise_odr_type_t odr);

public:
    icm20600(gleos::i2c::block &block);

    /**
     * Enable or disable the FIFO.
     * 
     * @param True if FIFO should be enabled, false otherwise.
     */
    void enable_fifo(bool enable);

    virtual bool driver_is_alive() override;
    virtual void driver_reset() override;
    virtual bool driver_set_power_mode(driver::power_mode mode) override;

    void read_acc_vector3(int16_t &x, int16_t &y, int16_t &z);
    void read_gyro_vector3(int16_t &x, int16_t &y, int16_t &z);
    void read_temperature(int16_t &temp);
};
