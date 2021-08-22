/**
 * Microcontroller firmware.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "icm20600.h"

#include "gleos/gleos.h"

#define I2C_ADDRESS 0x69

#define ICM20600_XG_OFFS_TC_H 0x04
#define ICM20600_XG_OFFS_TC_L 0x05
#define ICM20600_YG_OFFS_TC_H 0x07
#define ICM20600_YG_OFFS_TC_L 0x08
#define ICM20600_ZG_OFFS_TC_H 0x0a
#define ICM20600_ZG_OFFS_TC_L 0x0b
#define ICM20600_SELF_TEST_X_ACCEL 0x0d
#define ICM20600_SELF_TEST_Y_ACCEL 0x0e
#define ICM20600_SELF_TEST_Z_ACCEL 0x0f
#define ICM20600_XG_OFFS_USRH 0x13
#define ICM20600_XG_OFFS_USRL 0x14
#define ICM20600_YG_OFFS_USRH 0x15
#define ICM20600_YG_OFFS_USRL 0x16
#define ICM20600_ZG_OFFS_USRH 0x17
#define ICM20600_ZG_OFFS_USRL 0x18
#define ICM20600_SMPLRT_DIV 0x19
#define ICM20600_CONFIG 0x1a
#define ICM20600_GYRO_CONFIG 0x1b
#define ICM20600_ACCEL_CONFIG 0x1c
#define ICM20600_ACCEL_CONFIG2 0x1d
#define ICM20600_GYRO_LP_MODE_CFG 0x1e
#define ICM20600_ACCEL_WOM_X_THR 0x20
#define ICM20600_ACCEL_WOM_Y_THR 0x21
#define ICM20600_ACCEL_WOM_Z_THR 0x22
#define ICM20600_FIFO_EN 0x23
#define ICM20600_FSYNC_INT 0x36
#define ICM20600_INT_PIN_CFG 0x37
#define ICM20600_INT_ENABLE 0x38
#define ICM20600_FIFO_WM_INT_STATUS 0x39
#define ICM20600_INT_STATUS 0x3a
#define ICM20600_ACCEL_XOUT_H 0x3b
#define ICM20600_ACCEL_XOUT_L 0x3c
#define ICM20600_ACCEL_YOUT_H 0x3d
#define ICM20600_ACCEL_YOUT_L 0x3e
#define ICM20600_ACCEL_ZOUT_H 0x3f
#define ICM20600_ACCEL_ZOUT_L 0x40
#define ICM20600_TEMP_OUT_H 0x41
#define ICM20600_TEMP_OUT_L 0x42
#define ICM20600_GYRO_XOUT_H 0x43
#define ICM20600_GYRO_XOUT_L 0x44
#define ICM20600_GYRO_YOUT_H 0x45
#define ICM20600_GYRO_YOUT_L 0x46
#define ICM20600_GYRO_ZOUT_H 0x47
#define ICM20600_GYRO_ZOUT_L 0x48
#define ICM20600_SELF_TEST_X_GYRO 0x50
#define ICM20600_SELF_TEST_Y_GYRO 0x51
#define ICM20600_SELF_TEST_Z_GYRO 0x52
#define ICM20600_FIFO_WM_TH1 0x60
#define ICM20600_FIFO_WM_TH2 0x61
#define ICM20600_SIGNAL_PATH_RESET 0x68
#define ICM20600_ACCEL_INTEL_CTRL 0x69
#define ICM20600_USER_CTRL 0x6A
#define ICM20600_PWR_MGMT_1 0x6b
#define ICM20600_PWR_MGMT_2 0x6c
#define ICM20600_I2C_IF 0x70
#define ICM20600_FIFO_COUNTH 0x72
#define ICM20600_FIFO_COUNTL 0x73
#define ICM20600_FIFO_R_W 0x74
#define ICM20600_WHO_AM_I 0x75
#define ICM20600_XA_OFFSET_H 0x77
#define ICM20600_XA_OFFSET_L 0x78
#define ICM20600_YA_OFFSET_H 0x7a
#define ICM20600_YA_OFFSET_L 0x7b
#define ICM20600_ZA_OFFSET_H 0x7d
#define ICM20600_ZA_OFFSET_L 0x7e

#define ICM20600_FIFO_EN_BIT (1 << 6)
#define ICM20600_FIFO_RST_BIT (1 << 2)
#define ICM20600_RESET_BIT (1 << 0)
#define ICM20600_DEVICE_RESET_BIT (1 << 7)

icm20600::icm20600(gleos::i2c::block &block)
    : gleos::i2c::driver{block, I2C_ADDRESS}
{
    // Configuration
    m_i2c.write_register_byte(ICM20600_CONFIG, 0x00);

    // Disable the FIFO by default.
    enable_fifo(false);

    // set default power mode
    set_power_mode(power_mode::icm_6axis_low_noise);

    // Default accelerator configuration.
    set_acc_scale_range(RANGE_16G);
    set_acc_output_data_rate(ACC_RATE_1K_BW_420);
    set_acc_average_sample(ACC_AVERAGE_4);

    // Default gyroscope configuration.
    set_gyro_scale_range(RANGE_1K_DPS);
    set_gyro_output_data_rate(GYRO_RATE_1K_BW_176);
    set_gyro_average_sample(GYRO_AVERAGE_1);
}

void icm20600::enable_fifo(bool enable)
{
    m_i2c.write_register_byte(ICM20600_FIFO_EN, enable ? ICM20600_FIFO_EN_BIT : 0x00);
}

void icm20600::set_power_mode(power_mode mode)
{
    uint8_t data_pwr1 = m_i2c.read_register_byte(ICM20600_PWR_MGMT_1);
    data_pwr1 &= 0x8f; // 0b10001111

    uint8_t data_pwr2 = 0x00;

    // When set to ‘1’ low-power gyroscope mode is enabled. Default setting is 0
    uint8_t data_gyro_lp = m_i2c.read_register_byte(ICM20600_GYRO_LP_MODE_CFG);
    data_gyro_lp &= 0x7f; // 0b01111111

    switch (mode)
    {
    case power_mode::icm_sleep_mode:
        data_pwr1 |= 0x40; // set 0b01000000
        break;

    case power_mode::icm_standyby_mode:
        data_pwr1 |= 0x10; // set 0b00010000
        data_pwr2 = 0x38;  // 0x00111000 disable acc
        break;

    case power_mode::icm_acc_low_power:
        data_pwr1 |= 0x20; // set bit5 0b00100000
        data_pwr2 = 0x07;  //0x00000111 disable gyro
        break;

    case power_mode::icm_acc_low_noise:
        data_pwr1 |= 0x00;
        data_pwr2 = 0x07; //0x00000111 disable gyro
        break;

    case power_mode::icm_gyro_low_power:
        data_pwr1 |= 0x00; // dont set bit5 0b00000000
        data_pwr2 = 0x38;  // 0x00111000 disable acc
        data_gyro_lp |= 0x80;
        break;

    case power_mode::icm_gyro_low_noise:
        data_pwr1 |= 0x00;
        data_pwr2 = 0x38; // 0x00111000 disable acc
        break;

    case power_mode::icm_6axis_low_power:
        data_pwr1 |= 0x00; // dont set bit5 0b00100000
        data_gyro_lp |= 0x80;
        break;

    case power_mode::icm_6axis_low_noise:
        data_pwr1 |= 0x00;
        break;
    }

    m_i2c.write_register_byte(ICM20600_PWR_MGMT_1, data_pwr1);
    m_i2c.write_register_byte(ICM20600_PWR_MGMT_2, data_pwr2);
    m_i2c.write_register_byte(ICM20600_GYRO_LP_MODE_CFG, data_gyro_lp);
}

void icm20600::set_acc_scale_range(acc_scale_type_t range)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_ACCEL_CONFIG);
    data &= 0xe7; // 0b11100111

    switch (range)
    {
    case RANGE_2G:
        data |= 0x00; // 0bxxx00xxx
        m_acc_scale = 4000;
        break;

    case RANGE_4G:
        data |= 0x08; // 0bxxx01xxx
        m_acc_scale = 8000;
        break;

    case RANGE_8G:
        data |= 0x10; // 0bxxx10xxx
        m_acc_scale = 16000;
        break;

    case RANGE_16G:
        data |= 0x18; // 0bxxx11xxx
        m_acc_scale = 32000;
        break;

    default:
        break;
    }

    m_i2c.write_register_byte(ICM20600_ACCEL_CONFIG, data);
}

void icm20600::set_acc_output_data_rate(acc_lownoise_odr_type_t odr)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_ACCEL_CONFIG2);
    data &= 0xf0; // 0b11110000

    switch (odr)
    {
    case ACC_RATE_4K_BW_1046:
        data |= 0x08;
        break;

    case ACC_RATE_1K_BW_420:
        data |= 0x07;
        break;

    case ACC_RATE_1K_BW_218:
        data |= 0x01;
        break;

    case ACC_RATE_1K_BW_99:
        data |= 0x02;
        break;

    case ACC_RATE_1K_BW_44:
        data |= 0x03;
        break;

    case ACC_RATE_1K_BW_21:
        data |= 0x04;
        break;

    case ACC_RATE_1K_BW_10:
        data |= 0x05;
        break;

    case ACC_RATE_1K_BW_5:
        data |= 0x06;
        break;
    }

    m_i2c.write_register_byte(ICM20600_ACCEL_CONFIG2, data);
}

// Averaging filter only applies to low power mode.
void icm20600::set_acc_average_sample(acc_averaging_sample_type_t sample)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_ACCEL_CONFIG2);
    data &= 0xcf; // & 0b11001111

    switch (sample)
    {
    case ACC_AVERAGE_4:
        data |= 0x00; // 0bxx00xxxx
        break;

    case ACC_AVERAGE_8:
        data |= 0x10; // 0bxx01xxxx
        break;

    case ACC_AVERAGE_16:
        data |= 0x20; // 0bxx10xxxx
        break;

    case ACC_AVERAGE_32:
        data |= 0x30; // 0bxx11xxxx
        break;
    }

    m_i2c.write_register_byte(ICM20600_ACCEL_CONFIG2, data);
}

void icm20600::set_gyro_scale_range(gyro_scale_type_t range)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_GYRO_CONFIG);
    data &= 0xe7; // 0b11100111

    switch (range)
    {
    case RANGE_250_DPS:
        data |= 0x00; // 0bxxx00xxx
        m_gyro_scale = 500;
        break;

    case RANGE_500_DPS:
        data |= 0x08; // 0bxxx00xxx
        m_gyro_scale = 1000;
        break;

    case RANGE_1K_DPS:
        data |= 0x10; // 0bxxx10xxx
        m_gyro_scale = 2000;
        break;

    case RANGE_2K_DPS:
        data |= 0x18; // 0bxxx11xxx
        m_gyro_scale = 4000;
        break;
    }

    m_i2c.write_register_byte(ICM20600_GYRO_CONFIG, data);
}

// Averaging filter only applies to low power mode.
void icm20600::set_gyro_average_sample(gyro_averaging_sample_type_t sample)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_GYRO_LP_MODE_CFG);
    data &= 0x8f; // 0b10001111

    switch (sample)
    {
    case GYRO_AVERAGE_1:
        data |= 0x00; // 0bx000xxxx
        break;

    case GYRO_AVERAGE_2:
        data |= 0x10; // 0bx001xxxx
        break;

    case GYRO_AVERAGE_4:
        data |= 0x20; // 0bx010xxxx
        break;

    case GYRO_AVERAGE_8:
        data |= 0x30; // 0bx011xxxx
        break;

    case GYRO_AVERAGE_16:
        data |= 0x40; // 0bx100xxxx
        break;

    case GYRO_AVERAGE_32:
        data |= 0x50; // 0bx101xxxx
        break;

    case GYRO_AVERAGE_64:
        data |= 0x60;
        break;

    case GYRO_AVERAGE_128:
        data |= 0x70;
        break;

    default:
        break;
    }

    m_i2c.write_register_byte(ICM20600_GYRO_LP_MODE_CFG, data);
}

void icm20600::set_gyro_output_data_rate(gyro_lownoise_odr_type_t odr)
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_CONFIG);
    data &= 0xf8; // DLPF_CFG[2:0] 0b11111000

    switch (odr)
    {
    case GYRO_RATE_8K_BW_3281:
        data |= 0x07;
        break;
    case GYRO_RATE_8K_BW_250:
        data |= 0x00;
        break;
    case GYRO_RATE_1K_BW_176:
        data |= 0x01;
        break;
    case GYRO_RATE_1K_BW_92:
        data |= 0x02;
        break;
    case GYRO_RATE_1K_BW_41:
        data |= 0x03;
        break;
    case GYRO_RATE_1K_BW_20:
        data |= 0x04;
        break;
    case GYRO_RATE_1K_BW_10:
        data |= 0x05;
        break;
    case GYRO_RATE_1K_BW_5:
        data |= 0x06;
        break;
    }

    m_i2c.write_register_byte(ICM20600_CONFIG, data);
}

bool icm20600::driver_is_alive()
{
    return m_i2c.read_register_byte(ICM20600_WHO_AM_I) == 0x11;
}

void icm20600::read_acc_vector3(int16_t &x, int16_t &y, int16_t &z)
{
    uint8_t buffer[] = {0, 0, 0, 0, 0, 0};
    m_i2c.read_register(ICM20600_ACCEL_XOUT_H, buffer, sizeof(buffer));

    auto x_raw = gleos::buffer_to_i16(&buffer[0]);
    auto y_raw = gleos::buffer_to_i16(&buffer[2]);
    auto z_raw = gleos::buffer_to_i16(&buffer[4]);

    x = (x_raw * m_acc_scale) >> 16;
    y = (y_raw * m_acc_scale) >> 16;
    z = (z_raw * m_acc_scale) >> 16;
}

void icm20600::read_gyro_vector3(int16_t &x, int16_t &y, int16_t &z)
{
    uint8_t buffer[] = {0, 0, 0, 0, 0, 0};
    m_i2c.read_register(ICM20600_GYRO_XOUT_H, buffer, sizeof(buffer));

    auto x_raw = gleos::buffer_to_i16(&buffer[0]);
    auto y_raw = gleos::buffer_to_i16(&buffer[2]);
    auto z_raw = gleos::buffer_to_i16(&buffer[4]);

    x = (x_raw * m_gyro_scale) >> 16;
    y = (y_raw * m_gyro_scale) >> 16;
    z = (z_raw * m_gyro_scale) >> 16;
}

void icm20600::read_temperature(int16_t &temp)
{
    uint8_t buffer[] = {0, 0};
    m_i2c.read_register(ICM20600_TEMP_OUT_H, buffer, sizeof(buffer));

    auto temp_raw = gleos::buffer_to_i16(&buffer[0]);

    const auto room_temperature = 25;
    temp = temp_raw / 327 + room_temperature;
}

bool icm20600::driver_set_power_mode(driver::power_mode mode)
{
    switch (mode)
    {

    case driver::power_mode::sleep:
        set_power_mode(power_mode::icm_sleep_mode);
        return true;

    case driver::power_mode::normal:
        set_power_mode(power_mode::icm_6axis_low_noise);
        return true;
    }

    return false;
}

void icm20600::driver_reset()
{
    uint8_t data = m_i2c.read_register_byte(ICM20600_USER_CTRL);

    data &= 0xfe; // ICM20600_USER_CTRL[0] 0b11111110
    data |= ICM20600_RESET_BIT;

    m_i2c.write_register_byte(ICM20600_USER_CTRL, data);
}
