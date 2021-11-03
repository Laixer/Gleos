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
#include "driver.h"

#include "hardware/spi.h"

namespace gleos
{
    namespace spi
    {
        struct block
        {
            friend class bus;

            block(spi_inst_t *instance,
                  int port_mosi,
                  int port_simo,
                  int port_sclk,
                  int port_cs,
                  int baudrate);
            ~block();

        private:
            spi_inst_t *m_instance;
            int m_port_cs;
        };

        // TODO: Do something with the errors.
        class bus
        {
            block &m_block;

        public:
            /* Create new pulse modulation instance. */
            bus(block &block);

            void select();
            void deselect();

            int write(uint8_t *data, size_t len);
            int write_transaction(uint8_t *command, size_t command_len, uint8_t *data, size_t len);

            int read(uint8_t *data, size_t len);
            int read_transaction(uint8_t *command, size_t command_len, uint8_t *data, size_t len);
        };

        /**
         * SPI driver interface.
         * 
         * It is recommended that all SPI device drivers
         * inherit from this base class.
         */
        class driver : public gleos::driver
        {
        protected:
            bus m_spi;

        public:
            /**
             * Construct spi_driver instance.
             * 
             * @param block     The SPI allocated block.
             */
            driver(block &block)
                : m_spi{bus{block}}
            {
            }
        };
    }
}
