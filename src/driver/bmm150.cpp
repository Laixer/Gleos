/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "bmm150.h"

#define I2C_ADDRESS 0x13

/**\name API success code */
#define BMM150_OK (0)

/**\name API error codes */
#define BMM150_E_ID_NOT_CONFORM (-1)
#define BMM150_E_INVALID_CONFIG (-2)
// #define BMM150_E_ID_WRONG		    (-3)

/**\name API warning codes */
#define BMM150_W_NORMAL_SELF_TEST_YZ_FAIL INT8_C(1)
#define BMM150_W_NORMAL_SELF_TEST_XZ_FAIL INT8_C(2)
#define BMM150_W_NORMAL_SELF_TEST_Z_FAIL INT8_C(3)
#define BMM150_W_NORMAL_SELF_TEST_XY_FAIL INT8_C(4)
#define BMM150_W_NORMAL_SELF_TEST_Y_FAIL INT8_C(5)
#define BMM150_W_NORMAL_SELF_TEST_X_FAIL INT8_C(6)
#define BMM150_W_NORMAL_SELF_TEST_XYZ_FAIL INT8_C(7)
#define BMM150_W_ADV_SELF_TEST_FAIL INT8_C(8)

/**\name CHIP ID & SOFT RESET VALUES      */
#define BMM150_CHIP_ID 0x32
#define BMM150_SET_SOFT_RESET 0x82

/**\name POWER MODE DEFINTIONS      */
#define BMM150_NORMAL_MODE 0x00
#define BMM150_FORCED_MODE 0x01
#define BMM150_SLEEP_MODE 0x03
#define BMM150_SUSPEND_MODE 0x04

/**\name PRESET MODE DEFINITIONS  */
#define BMM150_PRESETMODE_LOWPOWER 0x01
#define BMM150_PRESETMODE_REGULAR 0x02
#define BMM150_PRESETMODE_HIGHACCURACY 0x03
#define BMM150_PRESETMODE_ENHANCED 0x04

/**\name Power mode settings  */
#define BMM150_POWER_CNTRL_DISABLE 0x00
#define BMM150_POWER_CNTRL_ENABLE 0x01

/**\name Sensor delay time settings  */
#define BMM150_SOFT_RESET_DELAY (1)
#define BMM150_NORMAL_SELF_TEST_DELAY (2)
#define BMM150_START_UP_TIME (3)
#define BMM150_ADV_SELF_TEST_DELAY (4)

/**\name ENABLE/DISABLE DEFINITIONS  */
#define BMM150_XY_CHANNEL_ENABLE 0x00
#define BMM150_XY_CHANNEL_DISABLE 0x03

/**\name Register Address */
#define BMM150_CHIP_ID_ADDR 0x40
#define BMM150_DATA_X_LSB 0x42
#define BMM150_DATA_X_MSB 0x43
#define BMM150_DATA_Y_LSB 0x44
#define BMM150_DATA_Y_MSB 0x45
#define BMM150_DATA_Z_LSB 0x46
#define BMM150_DATA_Z_MSB 0x47
#define BMM150_DATA_READY_STATUS 0x48
#define BMM150_INTERRUPT_STATUS 0x4A
#define BMM150_POWER_CONTROL_ADDR 0x4B
#define BMM150_OP_MODE_ADDR 0x4C
#define BMM150_INT_CONFIG_ADDR 0x4D
#define BMM150_AXES_ENABLE_ADDR 0x4E
#define BMM150_LOW_THRESHOLD_ADDR 0x4F
#define BMM150_HIGH_THRESHOLD_ADDR 0x50
#define BMM150_REP_XY_ADDR 0x51
#define BMM150_REP_Z_ADDR 0x52

/**\name DATA RATE DEFINITIONS  */
#define BMM150_DATA_RATE_10HZ (0x00)
#define BMM150_DATA_RATE_02HZ (0x01)
#define BMM150_DATA_RATE_06HZ (0x02)
#define BMM150_DATA_RATE_08HZ (0x03)
#define BMM150_DATA_RATE_15HZ (0x04)
#define BMM150_DATA_RATE_20HZ (0x05)
#define BMM150_DATA_RATE_25HZ (0x06)
#define BMM150_DATA_RATE_30HZ (0x07)

/**\name TRIM REGISTERS      */
/* Trim Extended Registers */
#define BMM150_DIG_X1 UINT8_C(0x5D)
#define BMM150_DIG_Y1 UINT8_C(0x5E)
#define BMM150_DIG_Z4_LSB UINT8_C(0x62)
#define BMM150_DIG_Z4_MSB UINT8_C(0x63)
#define BMM150_DIG_X2 UINT8_C(0x64)
#define BMM150_DIG_Y2 UINT8_C(0x65)
#define BMM150_DIG_Z2_LSB UINT8_C(0x68)
#define BMM150_DIG_Z2_MSB UINT8_C(0x69)
#define BMM150_DIG_Z1_LSB UINT8_C(0x6A)
#define BMM150_DIG_Z1_MSB UINT8_C(0x6B)
#define BMM150_DIG_XYZ1_LSB UINT8_C(0x6C)
#define BMM150_DIG_XYZ1_MSB UINT8_C(0x6D)
#define BMM150_DIG_Z3_LSB UINT8_C(0x6E)
#define BMM150_DIG_Z3_MSB UINT8_C(0x6F)
#define BMM150_DIG_XY2 UINT8_C(0x70)
#define BMM150_DIG_XY1 UINT8_C(0x71)

/**\name PRESET MODES - REPETITIONS-XY RATES */
#define BMM150_LOWPOWER_REPXY (1)
#define BMM150_REGULAR_REPXY (4)
#define BMM150_ENHANCED_REPXY (7)
#define BMM150_HIGHACCURACY_REPXY (23)

/**\name PRESET MODES - REPETITIONS-Z RATES */
#define BMM150_LOWPOWER_REPZ (2)
#define BMM150_REGULAR_REPZ (14)
#define BMM150_ENHANCED_REPZ (26)
#define BMM150_HIGHACCURACY_REPZ (82)

/**\name Macros for bit masking */
#define BMM150_PWR_CNTRL_MSK (0x01)

#define BMM150_CONTROL_MEASURE_MSK (0x38)
#define BMM150_CONTROL_MEASURE_POS (0x03)

#define BMM150_POWER_CONTROL_BIT_MSK (0x01)
#define BMM150_POWER_CONTROL_BIT_POS (0x00)

#define BMM150_OP_MODE_MSK (0x06)
#define BMM150_OP_MODE_POS (0x01)

#define BMM150_ODR_MSK (0x38)
#define BMM150_ODR_POS (0x03)

#define BMM150_DATA_X_MSK (0xF8)
#define BMM150_DATA_X_POS (0x03)

#define BMM150_DATA_Y_MSK (0xF8)
#define BMM150_DATA_Y_POS (0x03)

#define BMM150_DATA_Z_MSK (0xFE)
#define BMM150_DATA_Z_POS (0x01)

#define BMM150_DATA_RHALL_MSK (0xFC)
#define BMM150_DATA_RHALL_POS (0x02)

#define BMM150_SELF_TEST_MSK (0x01)

#define BMM150_ADV_SELF_TEST_MSK (0xC0)
#define BMM150_ADV_SELF_TEST_POS (0x06)

#define BMM150_DRDY_EN_MSK (0x80)
#define BMM150_DRDY_EN_POS (0x07)

#define BMM150_INT_PIN_EN_MSK (0x40)
#define BMM150_INT_PIN_EN_POS (0x06)

#define BMM150_DRDY_POLARITY_MSK (0x04)
#define BMM150_DRDY_POLARITY_POS (0x02)

#define BMM150_INT_LATCH_MSK (0x02)
#define BMM150_INT_LATCH_POS (0x01)

#define BMM150_INT_POLARITY_MSK (0x01)

#define BMM150_DATA_OVERRUN_INT_MSK (0x80)
#define BMM150_DATA_OVERRUN_INT_POS (0x07)

#define BMM150_OVERFLOW_INT_MSK (0x40)
#define BMM150_OVERFLOW_INT_POS (0x06)

#define BMM150_HIGH_THRESHOLD_INT_MSK (0x38)
#define BMM150_HIGH_THRESHOLD_INT_POS (0x03)

#define BMM150_LOW_THRESHOLD_INT_MSK (0x07)

#define BMM150_DRDY_STATUS_MSK (0x01)

/**\name OVERFLOW DEFINITIONS  */
#define BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL (-4096)
#define BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL (-16384)
#define BMM150_OVERFLOW_OUTPUT (-32768)
#define BMM150_NEGATIVE_SATURATION_Z (-32767)
#define BMM150_POSITIVE_SATURATION_Z (32767)
#ifdef BMM150_USE_FLOATING_POINT
#define BMM150_OVERFLOW_OUTPUT_FLOAT 0.0f
#endif

/**\name Register read lengths	*/
#define BMM150_SELF_TEST_LEN (5)
#define BMM150_SETTING_DATA_LEN (8)
#define BMM150_XYZR_DATA_LEN (8)

/**\name Self test selection macros */
#define BMM150_NORMAL_SELF_TEST (0)
#define BMM150_ADVANCED_SELF_TEST (1)

/**\name Self test settings */
#define BMM150_DISABLE_XY_AXIS (0x03)
#define BMM150_SELF_TEST_REP_Z (0x04)

/**\name Advanced self-test current settings */
#define BMM150_DISABLE_SELF_TEST_CURRENT (0x00)
#define BMM150_ENABLE_NEGATIVE_CURRENT (0x02)
#define BMM150_ENABLE_POSITIVE_CURRENT (0x03)

/**\name Normal self-test status */
#define BMM150_SELF_TEST_STATUS_XYZ_FAIL (0x00)
#define BMM150_SELF_TEST_STATUS_SUCCESS (0x07)

/**\name Macro to SET and GET BITS of a register*/
#define BMM150_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) |             \
     ((data << bitname##_POS) & bitname##_MSK))

#define BMM150_GET_BITS(reg_data, bitname) ((reg_data & (bitname##_MSK)) >> \
                                            (bitname##_POS))

#define BMM150_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) |                   \
     (data & bitname##_MSK))

#define BMM150_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

bmm150::bmm150(gleos::i2c::block &block)
    : gleos::i2c::driver{block, I2C_ADDRESS}
{
    set_power_mode(BMM150_NORMAL_MODE);
    set_preset_mode(BMM150_PRESETMODE_LOWPOWER);
}

bool bmm150::driver_is_alive()
{
    return m_i2c.read_register_byte(BMM150_CHIP_ID_ADDR) == BMM150_CHIP_ID;
}

void bmm150::write_op_mode(uint8_t op_mode)
{
    uint8_t data = m_i2c.read_register_byte(BMM150_OP_MODE_ADDR);

    data = BMM150_SET_BITS(data, BMM150_OP_MODE, op_mode);

    m_i2c.write_register_byte(BMM150_OP_MODE_ADDR, data);
}

void bmm150::set_power_control_bit(uint8_t pwrcntrl_bit)
{
    uint8_t data = m_i2c.read_register_byte(BMM150_POWER_CONTROL_ADDR);

    data = BMM150_SET_BITS_POS_0(data, BMM150_PWR_CNTRL, pwrcntrl_bit);

    m_i2c.write_register_byte(BMM150_POWER_CONTROL_ADDR, data);
}

void bmm150::set_power_mode(uint8_t pwr_mode)
{
    /* Select the power mode to set */
    switch (pwr_mode)
    {

    case BMM150_NORMAL_MODE:
        /* If the sensor is in suspend mode put the device to sleep mode */
        set_power_control_bit(BMM150_POWER_CNTRL_ENABLE);
        /* write the op mode */
        write_op_mode(pwr_mode);
        break;

    case BMM150_FORCED_MODE:
        /* If the sensor is in suspend mode put the device to sleep mode */
        set_power_control_bit(BMM150_POWER_CNTRL_ENABLE);
        /* write the op mode */
        write_op_mode(pwr_mode);
        break;

    case BMM150_SLEEP_MODE:
        /* If the sensor is in suspend mode put the device to sleep mode */
        set_power_control_bit(BMM150_POWER_CNTRL_ENABLE);
        /* write the op mode */
        write_op_mode(pwr_mode);
        break;

    case BMM150_SUSPEND_MODE:
        /* Set the power control bit to zero */
        set_power_control_bit(BMM150_POWER_CNTRL_DISABLE);
        break;
    }
}

void bmm150::set_odr_xyz_rep(struct bmm150_settings settings)
{
    uint8_t data = m_i2c.read_register_byte(BMM150_OP_MODE_ADDR);

    data = BMM150_SET_BITS(data, BMM150_ODR, settings.data_rate);

    m_i2c.write_register_byte(BMM150_OP_MODE_ADDR, data);
    m_i2c.write_register_byte(BMM150_REP_XY_ADDR, settings.xy_rep);
    m_i2c.write_register_byte(BMM150_REP_Z_ADDR, settings.z_rep);
}

void bmm150::set_preset_mode(uint8_t preset_mode)
{
    struct bmm150_settings settings;

    switch (preset_mode)
    {

    case BMM150_PRESETMODE_LOWPOWER:
        /* Set the data rate x,y,z repetition for Low Power mode */
        settings.data_rate = BMM150_DATA_RATE_10HZ;
        settings.xy_rep = BMM150_LOWPOWER_REPXY;
        settings.z_rep = BMM150_LOWPOWER_REPZ;
        set_odr_xyz_rep(settings);
        break;

    case BMM150_PRESETMODE_REGULAR:
        /* Set the data rate x,y,z repetition for Regular mode */
        settings.data_rate = BMM150_DATA_RATE_10HZ;
        settings.xy_rep = BMM150_REGULAR_REPXY;
        settings.z_rep = BMM150_REGULAR_REPZ;
        set_odr_xyz_rep(settings);
        break;

    case BMM150_PRESETMODE_HIGHACCURACY:
        /* Set the data rate x,y,z repetition for High Accuracy mode */
        settings.data_rate = BMM150_DATA_RATE_20HZ;
        settings.xy_rep = BMM150_HIGHACCURACY_REPXY;
        settings.z_rep = BMM150_HIGHACCURACY_REPZ;
        set_odr_xyz_rep(settings);
        break;

    case BMM150_PRESETMODE_ENHANCED:
        /* Set the data rate x,y,z repetition for Enhanced Accuracy mode */
        settings.data_rate = BMM150_DATA_RATE_10HZ;
        settings.xy_rep = BMM150_ENHANCED_REPXY;
        settings.z_rep = BMM150_ENHANCED_REPZ;
        set_odr_xyz_rep(settings);
        break;
    }
}

void bmm150::read_mag_vector3(int16_t &x, int16_t &y, int16_t &z)
{
    int16_t msb_data;
    uint8_t data[BMM150_XYZR_DATA_LEN] = {0};

    m_i2c.read_register(BMM150_DATA_X_LSB, data, BMM150_XYZR_DATA_LEN);

    /* Mag X axis data */
    data[0] = BMM150_GET_BITS(data[0], BMM150_DATA_X);
    /* Shift the MSB data to left by 5 bits */
    /* Multiply by 32 to get the shift left by 5 value */
    msb_data = ((int16_t)((int8_t)data[1])) * 32;
    /* Raw mag X axis data */
    int16_t raw_datax = (int16_t)(msb_data | data[0]);

    /* Mag Y axis data */
    data[2] = BMM150_GET_BITS(data[2], BMM150_DATA_Y);
    /* Shift the MSB data to left by 5 bits */
    /* Multiply by 32 to get the shift left by 5 value */
    msb_data = ((int16_t)((int8_t)data[3])) * 32;
    /* Raw mag Y axis data */
    int16_t raw_datay = (int16_t)(msb_data | data[2]);

    /* Mag Z axis data */
    data[4] = BMM150_GET_BITS(data[4], BMM150_DATA_Z);
    /* Shift the MSB data to left by 7 bits */
    /* Multiply by 128 to get the shift left by 7 value */
    msb_data = ((int16_t)((int8_t)data[5])) * 128;
    /* Raw mag Z axis data */
    int16_t raw_dataz = (int16_t)(msb_data | data[4]);

    /* Mag R-HALL data */
    data[6] = BMM150_GET_BITS(data[6], BMM150_DATA_RHALL);
    int16_t raw_data_r = (uint16_t)(((uint16_t)data[7] << 6) | data[6]);

    /* Compensated Mag X data in int16_t format */
    // x = compensate_x(raw_datax, raw_data_r);
    x = raw_datax;
    /* Compensated Mag Y data in int16_t format */
    // y = compensate_y(raw_datay, raw_data_r);
    y = raw_datay;
    /* Compensated Mag Z data in int16_t format */
    // z = compensate_z(raw_dataz, raw_data_r);
    z = raw_dataz;
}

void bmm150::driver_reset()
{
    uint8_t data = m_i2c.read_register_byte(BMM150_POWER_CONTROL_ADDR);

    data |= BMM150_SET_SOFT_RESET;

    m_i2c.write_register_byte(BMM150_POWER_CONTROL_ADDR, data);
}
