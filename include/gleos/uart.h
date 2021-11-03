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

#include "hardware/uart.h"

#include <functional>

namespace gleos
{
    class uart
    {
        uart_inst_t *m_iface;

        std::function<void(uint8_t *buf, size_t len)> m_on_byte_routine;

        uint8_t m_buffer[1024];
        size_t m_buf_sz{0};

        void enable_irq();

        static void irq_handler();

    public:
        uart(uart_inst_t *iface, int port_tx, int port_rx, int baud_rate);
        uart(const uart &) = delete;
        ~uart();

        static void unset(uart_inst_t *iface);

        void on_byte(std::function<void(uint8_t *buf, size_t len)> func)
        {
            m_on_byte_routine = func;

            enable_irq();
        }

        inline bool rx_has_data() const noexcept
        {
            return uart_is_readable(m_iface);
        }

        void write_putc(char c);
        void write(uint8_t *buffer, size_t len);

        uint8_t read_byte();
        void read(uint8_t *buffer, size_t len);

        void operator<<(const std::string &str);
    };
}
