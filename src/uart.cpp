/**
 * Glonax Embedded Operating System.
 *
 * Copyright (C) 2021 Laixer Equipment B.V.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the included license.  See the LICENSE file for details.
 */

#include "gleos/uart.h"

#include "hardware/irq.h"
#include "hardware/gpio.h"

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

using namespace gleos;

uart::uart(uart_inst_t *iface, int port_tx, int port_rx, int baud_rate)
    : m_iface{iface}
{
    uart::unset(iface);

    // Initialize the UART with provided baud rate.
    uart_init(iface, baud_rate);

    // Bind the default pins to the UART.
    gpio_set_function(port_tx, GPIO_FUNC_UART);
    gpio_set_function(port_rx, GPIO_FUNC_UART);

    // Turn off UART flow control.
    uart_set_hw_flow(iface, false, false);

    // Use standard UART format.
    uart_set_format(iface, DATA_BITS, STOP_BITS, PARITY);

    // We're most likely to read blocks at once, so enable FIFO.
    uart_set_fifo_enabled(iface, true);
}

uart::~uart()
{
    uart::unset(m_iface);
}

void uart::unset(uart_inst_t *iface)
{
    uart_deinit(iface);
}

// #define ENABLE_IRQ 1

#if ENABLE_IRQ
static uart *m_static_weak_ref{nullptr};
#endif // ENABLE_IRQ

void uart::irq_handler()
{
#if ENABLE_IRQ
    std::array<uint8_t, 1024> buffer;
    size_t buffer_len = 0;

    while (uart_is_readable(m_static_weak_ref->m_iface))
    {
        buffer[buffer_len++] = uart_getc(m_static_weak_ref->m_iface);
    }

    m_static_weak_ref->m_on_byte_routine(buffer.data(), buffer_len);
#endif // ENABLE_IRQ
}

void uart::enable_irq()
{
#if ENABLE_IRQ
    // int UART_IRQ = UART_ID == m_iface ? UART0_IRQ : UART1_IRQ;
    // int UART_IRQ = UART0_IRQ;
    int UART_IRQ = UART0_IR1;

    irq_clear(UART_IRQ);
    irq_set_exclusive_handler(UART_IRQ, uart::irq_handler);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(m_iface, true, false);

    m_static_weak_ref = this;
#endif // ENABLE_IRQ
}

uint8_t uart::read_byte()
{
    return uart_getc(m_iface);
}

void uart::read(uint8_t *buffer, size_t len)
{
    uart_read_blocking(m_iface, buffer, len);
}

void uart::write_putc(char c)
{
    uart_putc(m_iface, c);
}

void uart::write(const uint8_t *buffer, size_t len)
{
    uart_write_blocking(m_iface, buffer, len);
}

void uart::operator<<(const std::string &str)
{
    uart_puts(m_iface, str.c_str());
}
