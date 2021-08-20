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

#include "hardware/i2c.h"

namespace gleos
{
    namespace i2c
    {
        /* I2C predefined baudrates */
        enum mode
        {
            standard_mode = 100,
            fast_mode = 400,
            fast_mode_plus = 1000,
        };

        struct block
        {
            friend class layer3;

            block(int port_sda, int port_scl, mode baudrate = mode::standard_mode);
            ~block();

        private:
            i2c_inst_t *m_instance;
        };

        // TODO: Do something with the errors.
        class layer3
        {
            block &m_block;
            uint8_t m_address;

        public:
            /* Create new pulse modulation instance. */
            layer3(block &block, uint8_t address);

            int write(uint8_t *data, size_t len);
            int write_register_byte(uint8_t reg, uint8_t data);

            int read(uint8_t *data, size_t len);
            int read_register(uint8_t reg, uint8_t *data, size_t len);

            uint8_t read_register_byte(uint8_t reg);
        };

        /**
         * I2C driver interface.
         * 
         * It is recommended that all I2C device drivers
         * inherit from this base class.
         */
        class driver : public gleos::driver
        {
        protected:
            layer3 m_i2c;

        public:
            /**
             * Construct i2c_driver instance.
             * 
             * @param block     The I2C allocated block.
             * @param address   Address of the device.
             */
            driver(block &block, uint8_t address)
                : m_i2c{layer3{block, address}}
            {
            }
        };
    }
}
