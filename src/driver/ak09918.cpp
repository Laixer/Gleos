/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "ak09918.h"

#define I2C_ADDRESS 0x0c
#define WIA_VENDOR 0x48

#define AK09918_WIA1 0x00  // Company ID (0x48)
#define AK09918_WIA2 0x01  // Device ID (0x0c)
#define AK09918_RSV1 0x02  // Reserved 1
#define AK09918_RSV2 0x03  // Reserved 2
#define AK09918_ST1 0x10   // DataStatus 1
#define AK09918_HXL 0x11   // X-axis data
#define AK09918_HXH 0x12   //
#define AK09918_HYL 0x13   // Y-axis data
#define AK09918_HYH 0x14   //
#define AK09918_HZL 0x15   // Z-axis data
#define AK09918_HZH 0x16   //
#define AK09918_TMPS 0x17  // Dummy
#define AK09918_ST2 0x18   // Datastatus 2
#define AK09918_CNTL1 0x30 // Dummy
#define AK09918_CNTL2 0x31 // Control settings
#define AK09918_CNTL3 0x32 // Control settings

#define AK09918_SRST_BIT 0x01 // Soft Reset
#define AK09918_HOFL_BIT 0x08 // Sensor Over Flow
#define AK09918_DOR_BIT 0x02  // Data Over Run
#define AK09918_DRDY_BIT 0x01 // Data Ready

// NOTE: Even though this device supports fast mode it looks like
//       standard mode has better performance.
ak09918::ak09918(gleos::i2c::block &block)
    : gleos::i2c::driver{block, I2C_ADDRESS}
{
    set_operation_mode(operation_mode::continuous_100hz);
}

void ak09918::set_operation_mode(operation_mode mode)
{
    m_i2c.write_register_byte(AK09918_CNTL2, mode);
}

bool ak09918::is_data_ready()
{
    return m_i2c.read_register_byte(AK09918_ST1) & AK09918_DRDY_BIT;
}

bool ak09918::is_data_skipped()
{
    return m_i2c.read_register_byte(AK09918_ST1) & AK09918_DOR_BIT;
}

bool ak09918::driver_is_alive()
{
    uint8_t buffer[] = {0, 0};
    m_i2c.read_register(AK09918_WIA1, buffer, sizeof(buffer));

    return buffer[0] == WIA_VENDOR && buffer[1] == I2C_ADDRESS;
}

// NOTE: We're also reading the ST2 register which unlocks
//       the mutex to the HXL..HZH registers.
// TODO: Will keep spinning when mode is single measurement.
void ak09918::read_mag_vector3(int16_t &x, int16_t &y, int16_t &z)
{
    while (true)
    {
        uint8_t buffer[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        m_i2c.read_register(AK09918_ST1, buffer, sizeof(buffer));

        // If the measurement was not yet ready to set the shift
        // register then spend another cycle and check again.
        if (!(buffer[0] & AK09918_DRDY_BIT))
        {
            continue;
        }

        auto axis_x = gleos::buffer_to_i16le(&buffer[1]);
        auto axis_y = gleos::buffer_to_i16le(&buffer[3]);
        auto axis_z = gleos::buffer_to_i16le(&buffer[5]);

        // Magnetic field overflow, ignore this measurement.
        if (buffer[8] & AK09918_HOFL_BIT)
        {
            continue;
        }

        // Calculate the magnetic flux density from the signed 16-bit normal.
        x = (axis_x * 15) / 100;
        y = (axis_y * 15) / 100;
        z = (axis_z * 15) / 100;

        break;
    }
}

bool ak09918::driver_set_power_mode(power_mode mode)
{
    switch (mode)
    {

    case power_mode::suspend:
        set_operation_mode(operation_mode::power_down);
        return true;

    case power_mode::normal:
        set_operation_mode(operation_mode::continuous_100hz);
        return true;
    }

    return false;
}

void ak09918::driver_reset()
{
    m_i2c.write_register_byte(AK09918_CNTL3, AK09918_SRST_BIT);
}
