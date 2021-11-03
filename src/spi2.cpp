/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/spi.h"

#include "hardware/gpio.h"

using namespace gleos::spi;

block::block(spi_inst_t *instance, int port_mosi, int port_simo, int port_sclk, int port_cs, int baudrate)
    : m_instance{instance}, m_port_cs{port_cs}
{
    // gpio_pull_up(port_sda);
    // gpio_pull_up(port_scl);
    ///
    spi_init(instance, baudrate * 1000000);

    gpio_set_function(port_mosi, GPIO_FUNC_SPI);
    gpio_set_function(port_simo, GPIO_FUNC_SPI);
    gpio_set_function(port_sclk, GPIO_FUNC_SPI);

    gpio_init(port_cs);
    gpio_set_dir(port_cs, GPIO_OUT);
    gpio_put(port_cs, 1);
}

block::~block()
{
    spi_deinit(m_instance);
}

bus::bus(block &block)
    : m_block{block}
{
}

void bus::select()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(m_block.m_port_cs, 0);
    asm volatile("nop \n nop \n nop");
}

void bus::deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(m_block.m_port_cs, 1);
    asm volatile("nop \n nop \n nop");
}

int bus::write(uint8_t *data, size_t len)
{
    return spi_write_blocking(m_block.m_instance, data, len);
}

int bus::write_transaction(uint8_t *command, size_t command_len, uint8_t *data, size_t len)
{
    select();
    write(command, command_len);
    write(data, len);
    deselect();

    return 0;
}

int bus::read(uint8_t *data, size_t len)
{
    return spi_read_blocking(m_block.m_instance, 0, data, len);
}

int bus::read_transaction(uint8_t *command, size_t command_len, uint8_t *data, size_t len)
{
    select();
    write(command, command_len);
    read(data, len);
    deselect();

    return 0;
}
