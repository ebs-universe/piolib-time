/*
    Copyright (c) 
      (c) 2016-2018 Chintalagiri Shashank, Firefly Aerospace Pvt.Ltd.
      (c) 2010-2011 Chintalagiri Shashank, Jugnu, IIT Kanpur

    This file is part of
    Embedded bootstraps : time library

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/**
 * @file time.h
 * @brief Time related functions for embebedded systems.
 * 
 * While this library uses int64_t with the intent to support
 * negative timestamps, most functions would do not support this and 
 * will break.
 * 
 * This library uses the following UCDM registers: 
 * 
 * Base Address: TIME_BASE_ADDRESS
 * 
 * System Time is the current time of the device. This is returned directly
 * from the system time in the same format, ie, int64_t.
 * 
 * Address | Description             | Access Type
 * --------|-------------------------|--------------
 *    0    | System Time, Register 0 | Read, Pointer
 *    1    | System Time, Register 1 | Read, Pointer
 *    2    | System Time, Register 2 | Read, Pointer
 *    3    | System Time, Register 3 | Read, POinter
 * 
 * If synchronization is enabled by TIME_ENABLE_SYNC, the following additional
 * registers are used
 *
 * Address | Description             | Access Type
 * --------|-------------------------|--------------
 *    4    | System Time Read Hook   | Read, Func
 *    5    | Reserved                | None
 *    6    | Time Sync Register 1    | Write
 *    7    | Time Sync Register 2    | Write
 *    8    | Time Sync Register 3    | Write
 *    9    | Time Sync Register 4    | Write, Handle
 * 
 * 
 * This library provides the following descriptors: 
 * 
 * DESCRIPTOR_TAG_LIBVERSION : Time Library Version, per EBS version format
 * DESCRIPTOR_TAG_TIME_EPOCH : Configured Epoch Time
 * 
 * @see time.c
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <string.h>
#include "config.h"
#include <platform/cpu.h>
#include <ucdm/ucdm.h>

/**
 * @name Time Tick Calculations
 * 
 * These definitions configure the systick and prepare all the calculated names 
 * derived from it. 
 * 
 * This definition is informational only. Changing this number could have insidious 
 * consequences. 
 * 
 */
/**@{*/ 

#define TIME_SYSTICK_PERIOD_uS          1000

#define TIME_TICKS_PER_SECOND           (int32_t)(1000000 / TIME_SYSTICK_PERIOD_uS)
#define TIME_TICKS_PER_MINUTE           (TIME_TICKS_PER_SECOND * 60)
#define TIME_TICKS_PER_HOUR             (TIME_TICKS_PER_MINUTE * 60)
#define TIME_TICKS_PER_DAY              (TIME_TICKS_PER_HOUR * 24)

#define TIME_SECONDS_PER_MINUTE         (int32_t)(60)
#define TIME_SECONDS_PER_HOUR           (TIME_SECONDS_PER_MINUTE * 60)
#define TIME_SECONDS_PER_DAY            (TIME_SECONDS_PER_HOUR * 24)

/**@}*/

/**
 * @name Time Types
 * 
 * Various type definitions related to time storage and exchange.
 */
/**@{*/ 

/**
 * @brief System Time Storage Type
 * 
 * A signed int64_t type, stores a 'system' time as number of milliseconds 
 * since the epoch.  
 * 
 * The least count is based on the least count created by the system 
 * tick, and is expected to be 1ms.
 * 
 */
typedef int64_t tm_system_t;

/**
 * @brief System Timedelta Storage Type
 * 
 * This type stores a 'system' time difference. 
 * 
 * The least count is based on the least count created by the system 
 * tick, and is expected to be 1ms. 
 * 
 */
typedef int64_t tm_sdelta_t;

#ifndef TM_REAL_t_DEFINED
#define TM_REAL_t_DEFINED

/**
 * @brief Real Time Storage Type
 * 
 * This (struct) type stores a 'real' time. This type does not depend on 
 * the epoch and is independent of system time. 
 *
 * This form of time storage is always going to be expensive, and should
 * therefore be used sparingly. 
 * 
 * This type is not time-zone aware, and should always be interpreted as
 * UTC time (GMT+0000). See the documentation of time.h for further detail
 * about the interpretation of time by this library.
 * 
 * This library does no implicit validation of the data provided, and 
 * values outside expected ranges of each component of the real time 
 * can cause unexpected behaviour and crashes. If applications use 
 * uexternal or unsanitized sources to initizalize this type, then the 
 * application must also validate the input. One option to do so would
 * be to initialize the tm_real_t and then use tm_check_invalid_rtime().   
 * 
 */
typedef struct TM_REAL_t{
    uint8_t century;
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t millis; 
} tm_real_t;

#endif

/**
 * @brief Real Timedelta Storage Type
 * 
 * This (struct) type stores a 'real' time difference. 
 * 
 * This form of time storage is always going to be expensive, and 
 * should therefore be used sparingly. 
 * 
 */
typedef struct TM_RDELTA_t{
    uint8_t sgn;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint16_t days;
    uint16_t millis;
} tm_rdelta_t;


/**
 * @brief Epoch Change Handler Type
 * 
 * This (struct) type stores an epoch change handler, and must be 
 * instantiated and registered by consumers requiring epoch change 
 * notifications. 
 * 
 * Generally, an epoch change should be considered a rare occurence.
 * If an application is expecting epoch changes, then the application
 * can use the time delta provided to the handler to shift any stored
 * timestamps to the new epoch. This remains largely untested.
 * 
 * If the epoch change is not to be followed, then the change handler
 * will be provided 0 as the time delta. In such cases, applications 
 * must discard any stored timestamps. This usually would happen if 
 * the two epochs are fundamentally incompatible, or if the change 
 * involves more than just a shift in the epoch.
 * 
 * Note that epoch change handlers are run in an interrupt-disabled 
 * context, so they should be as quick as possible. 
 * 
 * The epoch change handlers should be called in increasing order of 
 * the priority member, and this could, in principle, be used by the 
 * application to sequence shifts of related timestamps. This is not 
 * implemented. 
 * 
 */
typedef struct TM_EPOCH_CHANGEHANDLER_t{
    struct TM_EPOCH_CHANGEHANDLER_t * next;
    uint8_t priority;
    void (* func)(tm_sdelta_t *);
}tm_epochchange_handler_t;

/**@}*/ 

/**
 * @name Time Containers
 * 
 * These containers are defined internally in the library.
 */
/**@{*/ 

extern volatile tm_system_t tm_current;
extern   tm_real_t tm_epoch;
extern      int8_t tm_leapseconds;
extern   tm_real_t tm_internal_epoch;
extern     int64_t tm_internal_epoch_offset;
extern     uint8_t use_epoch;
extern tm_epochchange_handler_t * epoch_handlers_root;

/**@}*/ 

/**
 * @name Time Initialization and Maintenance Functions
 * 
 */
/**@{*/

/**
 * @brief Intitialize time library constructs. 
 * 
 * The address provided to this function will be called TIME_BASE_ADDRESS for
 * the purposes of this library, though EBS applications using the devicemap 
 * may choose to call it DMAP_TIME_BASE_ADDRESS instead. All other UCDM 
 * addresses discussed in this library will be referenced to this base address
 * unless otherwise specified. 
 * 
 */
ucdm_addr_t tm_init(ucdm_addr_t ucdm_address);

/**
 * @brief Install the time library descriptor to the application.
 * 
 * The time application descriptor contains the library version number and is
 * installed to the application descriptors with the tag specified in UCDM's 
 * descriptor header as DESCRIPTOR_TAG_LIBVERSION. 
 * 
 * This does not effect the functionality of the time library in any way.
 */
void tm_install_descriptor(void);

/**
 * @brief Clear a tm_system_t instance.
 * 
 * @param stime Pointer to the tm_system_t to clear.
 */
void tm_clear_stime(tm_system_t* stime);

/**
 * @brief Clear a tm_sdelta_t instance.
 * 
 * @param sdelta Pointer to the time_sdelta_t to clear.
 */
void tm_clear_sdelta(tm_sdelta_t* sdelta);

/**
 * @brief Clear a tm_real_t instance.
 * 
 * @param rtime Pointer to the time_real_t to clear.
 */
void tm_clear_rtime(tm_real_t* rtime);

/**
 * @brief Clear a tm_rdelta_t instance.
 * 
 * @param rdelta Pointer to the time_delta_t to clear.
 */
void tm_clear_rdelta(tm_rdelta_t* rdelta);


/**
 * @brief Validate a tm_real_t instance.
 * 
 * Returns 0 for valid tm_real_t, non-zero for 
 * out of bounds.
 * 
 * @param rtime Pointer to the tm_rtime_t to validate.
 */
uint8_t tm_check_invalid_rtime(tm_real_t* rtime);

/**@}*/ 

/**
 * @name Basic Time Manipulation Functions
 * 
 * Basic time information manipulation functions. Use these
 * whenever possible, and resort to using the (higher level) 
 * Real time manipulation functions only when it's really necessary.
 * 
 */
/**@{*/ 

/**
 * @brief Get the current system time. 
 * 
 * @param stime Pointer to the location where the current system timestamp 
 *               should be copied to.
 */
static inline void tm_current_time(tm_system_t * stime);

static inline void tm_current_time(tm_system_t * stime){
    critical_enter();
    *stime = tm_current;
    critical_exit();
    return;
}

static inline int8_t tm_cmp_stime(tm_system_t * t1, tm_system_t * t2);

static inline int8_t tm_cmp_stime(tm_system_t * t1, tm_system_t * t2){
    if (*t1 < *t2){
        return (-1);
    }
    else if (*t1 > *t2){
        return (1);
    }
    return 0;
}

/**
 * @brief Get the difference between two system times as a system 
 *        time delta (t2 - t1)
 * 
 * Note that both times must be against the same epoch time.
 * 
 * @param t1 Pointer to the first timestamp (*tm_system_t)
 * @param t2 Pointer to the second timestamp (*tm_system_t)
 * @param sdelta Poiter to the location where the delta should be stored
 */
static inline void tm_get_sdelta(tm_system_t * t1, tm_system_t * t2, tm_sdelta_t * sdelta);

static inline void tm_get_sdelta(tm_system_t * t1, tm_system_t * t2, tm_sdelta_t * sdelta){
    *sdelta = (*t2 - *t1);
}


/**
 * @brief Apply a time difference to a time in system time. 
 * 
 * @param t Pointer to (a copy of) the original timestamp. This will be overwritten.
 * @param sdelta Pointer to the time difference to apply 
 */
static inline void tm_apply_sdelta(tm_system_t * t, tm_sdelta_t * sdelta);

static inline void tm_apply_sdelta(tm_system_t * t, tm_sdelta_t * sdelta){
    *t = *t + *sdelta;
}

/**@}*/ 


/**
 * @name Real Time Manipulation Functions
 * 
 * Functions to manipule Real time. Use these functions sparingly, 
 * and usually only when dealing with user-generated or externally
 * generated information. The further down this group of functions 
 * you go, the worse it's going to get.
 * 
 */
/**@{*/ 

/**
 * @brief Convert a tm_rdelta_t instance into systick units.
 * 
 * @param rdelta Pointer to the time difference in real time units.
 * @param sdelta Pointer to the tm_sdelta_t in which to store the result.
 */
void tm_sdelta_from_rdelta(tm_rdelta_t* rdelta, tm_sdelta_t* sdelta);

/**
 * @brief Convert a time difference in systick units into a real time difference.
 * 
 * @param sdelta Pointer to the time difference in system time units.
 * @param rdelta Pointer to the tm_rdelta_t in which to store the result.
 */
void tm_rdelta_from_sdelta(tm_sdelta_t* sdelta, tm_rdelta_t * rdelta);


/**
 * @brief Apply a real time difference to a time in systick units.
 * 
 * @param t Pointer to (a copy of) the original timestamp. This will be overwritten.
 * @param rdelta Pointer to the tm_rdelta_t in which the desired difference is stored.
 */
static inline void tm_apply_rdelta(tm_system_t * t, tm_rdelta_t * rdelta);

static inline void tm_apply_rdelta(tm_system_t * t, tm_rdelta_t * rdelta){
    tm_sdelta_t sdelta;
    tm_sdelta_from_rdelta(rdelta, &sdelta);
    tm_apply_sdelta(t, &sdelta);
}

/**
 * @brief Get a system timestamp from a real time, against the current epoch.
 * 
 * If the current epoch is not set, returns 0. Currently only supports 
 * epochs with frac = 0.
 * 
 * @todo Align to timegm. 
 * 
 * @param rtime The real time (as a pointer to a tm_real_t instance)
 * @param stime Pointer to the tm_system_t in which to store the result.
 */
void tm_stime_from_rtime(tm_real_t* rtime, tm_system_t* stime);

/**
 * @brief Get the real time from a system timestamp against the current epoch.
 * 
 * If the current epoch is not set, does nothing.
 * 
 * @note NOT IMPLEMENTED YET!
 * 
 * @todo Align to gmtime
 * 
 * @param stime The system time (as a pointer to a tm_system_t instance)
 * @param rtime Pointer to the tm_real_t in which to store the result.
 *              represented by stime.
 */
void tm_rtime_from_stime(tm_system_t* stime, tm_real_t* rtime);


/**@}*/ 

/**
 * @name Epoch Manipulation Functions
 * 
 * Functions to manipule the Epoch. These functions should be avoided in 
 * all but the most demanding of applications. The default configuration
 * of the library uses the Unix Epoch. 
 * 
 * Variable Epochs will most likely be useful in cases where a second time 
 * reference is maintained relative to the primary Epoch. This type of use
 * is not currently supported by this library, since tm_epoch is a global 
 * which is used by a number of functions. Passing tm_epoch around is going
 * to make it far more expensive than it already is. 
 * 
 * These implementations are here for the sake of completeness. If you 
 * don't actually used the functions, the compiler will get rid of them. 
 * 
 * In the future, it may be possible to adapt these implementations to 
 * provide one or more of the following : 
 * 
 *    - A variable epoch operating alongside a high resolution system
 *      clock. The high resolution of the clock will shorten the epoch 
 *      validity for the same size of the clock.
 *    - Allow the use of these functions to maintain secondary / 
 *      multiple epochs. An example use case could be the maintenance 
 *      of TLE Epochs. 
 */
/**@{*/ 

/**
 * @brief Set the epoch time from an external real time source. 
 * 
 * Note that for 'follow' to work, both the new epoch and the old one 
 * should be representable against the old epoch. 
 * 
 * Because this function touches timestamps across the system, and the
 * time variables are likely in an inconsistent state over the course 
 * of this operation, this function disables interrupts. 
 * 
 * @param rtime Pointer to the time_real_t in which the desired epoch
 *               is stored
 * @param follow Whether the system should try to adjust stored 
 *               timestamps to the new epoch.
 */
void tm_set_epoch(tm_real_t* rtime, uint8_t follow);

/**
 * @brief Register a callback for changes to the epoch. 
 * 
 * Use this if you are storing timestamps for any reason. If possible, 
 * invalidate your stored timestamps. If you need to retain that information, 
 * apply the provided delta. 
 * 
 * If the change is not intended to be followed, the delta reported will be 
 * zero. In this case, the function should discard all timestamp information.
 * 
 * @note This function should also be used to follow time sync where 
 *       applicable. 
 * 
 * @param handler Pointer to the epoch change handler.
 * 
 */
void tm_register_epoch_change_handler(tm_epochchange_handler_t * handler);

/**@}*/ 
#endif
