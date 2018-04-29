/* 
   Copyright (c) 
     (c) 2016      Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
   This file is part of
   Embedded bootstraps : Application Prototype
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file application.h
 * @brief Application configuration parameters.
 *
 * This file, and the file hierarchy that it includes, should define
 * all the parameters necessary for configuring all inlcuded C code. 
 * This hierarchy should not possess any functional code within itself, 
 * and is to be used only as an in-code configuration store. It should 
 * only add preprocessor files and definitions and should not create 
 * any compilable code. It should be safe to include within _any_ file, 
 * and when included, provides all the compile-time preprocessor switches
 * necessary to correctly compile / link the appropriate code. 
 * 
 * All other source files should include this file, with the exception of
 * files within the uC HAL implementation, which will include this file
 * via hal_uc_map.h, which additionally processes the definitions here 
 * to provide 'calculated' compile time parameters. 
 * 
 * The lower level files in this hierarchy should not be included 
 * separately, and the information present in them should be accessed
 * only though this file.
 */


#ifndef APPLICATION_H
#define APPLICATION_H

#include "app/config.h"
#include "bsp/hal/uc/types.h"

#define APP_ENABLE_BCIF         0
#define APP_ENABLE_UCDM         1

/**
 * @name Modbus Interface Configuration
 */
/**@{*/
    /**
    * @name Modbus Interface Configuration
    */
    /**@{*/ 
    #define APP_ENABLE_MODBUS               1
    #define MODBUS_PLUGGABLE_TRANSPORTS     0
    #define MODBUS_ENABLE_TRANSPORT_UART    0
    #define MODBUS_TRANSPORT_UART_INTFNUM   1
    #define MODBUS_ENABLE_TRANSPORT_USBCDC  1
    #define MODBUS_TRANSPORT_USBCDC_INTFNUM 1
    #define APP_MODBUS_TRANSPORT            MODBUS_USBCDC
    #define MB_DIAGNOSTICS                  0
    #define MB_SUPPORT_BITFUNCS             0
    #define MB_SUPPORT_CELOGFUNCS           0
    /**@}*/ 
/**@}*/ 

/**
 * @name Entropy & Random Configuration
 */
/**@{*/ 
    #define APP_ENTROPY_INTFNUM       0
    #define APP_ENABLE_RAND           1
    #define APP_RAND_GENERATOR        1
/**@}*/

/**
 * @name Time Configuration
 */
/**@{*/ 
    #define APP_ENABLE_SYSTICK        1
    #define APP_SYSTICK_TIMER_INTFNUM 1
    #define APP_SYSTICK_FREQ_Hz       1000

    #define APP_ENABLE_RTC            1
/**@}*/


// Core HAL Configuration. Should be calculated instead.
#define uC_USBCDC_ENABLED             1
#define uC_USBHID_ENABLED             0
#define uC_USBMSC_ENABLED             0
#define uC_USBPHDC_ENABLED            0

#define uC_TIMER0_ENABLED             1

#define uC_TIMER1_ENABLED             1
#define uC_TIMER1_TOP_IRQ_HANDLER     time_systick_handler

#define uC_UART0_ENABLED              0
#define uC_UART1_ENABLED              1
#define uC_UART1_BAUD                 256000

#include "bsp/board.h"
#include "bsp/uc_pum.h"
#include "devicemap.h"

#endif
