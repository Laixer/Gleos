/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/i2c.h"

#include "hardware/gpio.h"

using namespace gleos::i2c;

block::block(int port_sda, int port_scl, mode baudrate)
    : m_instance{i2c_default}
{
    i2c_init(i2c_default, baudrate * 1000);
    gpio_set_function(port_sda, GPIO_FUNC_I2C);
    gpio_set_function(port_scl, GPIO_FUNC_I2C);
    gpio_pull_up(port_sda);
    gpio_pull_up(port_scl);
}

block::~block()
{
    i2c_deinit(m_instance);
}

layer3::layer3(block &block, uint8_t address)
    : m_block{block}, m_address{address}
{
}

int layer3::write(uint8_t *data, size_t len)
{
    return i2c_write_blocking(m_block.m_instance, m_address, data, len, false);
}

int layer3::write_register_byte(uint8_t reg, uint8_t data)
{
    uint8_t buffer[] = {reg, data};
    return write(buffer, sizeof(buffer));
}

int layer3::read(uint8_t *data, size_t len)
{
    return i2c_read_blocking(m_block.m_instance, m_address, data, len, false);
}

int layer3::read_register(uint8_t reg, uint8_t *data, size_t len)
{
    auto ret = write(&reg, 1);
    if (!ret)
    {
        return ret;
    }
    return read(data, len);
}

uint8_t layer3::read_register_byte(uint8_t reg)
{
    uint8_t data = 0;
    read_register(reg, &data, 1);
    return data;
}
