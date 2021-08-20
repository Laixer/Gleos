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

namespace gleos
{
    /**
     * Driver interface.
     * 
     * It is recommended that all device drivers inherit
     * from this base or one of its specializations.
     */
    class driver
    {

    public:
        enum power_mode
        {
            /* The device will be powered-down. */
            suspend,
            /* The device will enter a low-power sleep mode. */
            sleep,
            /* The device will run in normal operational mode. */
            normal,
        };

    public:
        /**
         * Test if device is alive.
         * 
         * All drivers must find a way to implement this. Note that
         * some devices will only respond to communication when they
         * are activated.
         * 
         * @return True if the device is alive, false otherwise.
         */
        virtual bool driver_is_alive() = 0;

        /**
         * Reset device.
         * 
         * This method only has effect if it is supported
         * by the device driver.
         * 
         * It is common for a device to remain in a power-down
         * mode after reset. The generic 'normal' power mode
         * can be requested for any device or a dedicated
         * operation mode to re-activate the device.
         */
        virtual void driver_reset() {}

        /**
         * Move device into power mode.
         * 
         * This method only has effect if it is supported
         * by the device driver. Device drivers can also
         * partially support the operation.
         * 
         * @param mode  Requested power mode.
         * @return      Whether or not the requested power change
         *              was effective.
         */
        virtual bool driver_set_power_mode(power_mode mode)
        {
            return false;
        }
    };
} // gleos
