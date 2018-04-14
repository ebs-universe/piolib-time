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
 * @see time.c
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <string.h>
#include "config.h"

/**
 * @name Time Configuration Definitions
 * 
 * It would be nice to be able to provide these definitions from the 
 * applications, but that would require recompilation of the library. 
 * 
 * For the moment, these are defitions which the application must be 
 * built to respect / accomodate. 
 */
/**@{*/ 

#define TIME_DEFAULT_EPOCH_CENTURY      19
#define TIME_DEFAULT_EPOCH_YEAR         70 
#define TIME_DEFAULT_EPOCH_MONTH        1
#define TIME_DEFAULT_EPOCH_DATE         1
#define TIME_MAX_EPOCH_CHANGE_HANDLERS  2

// This definition is informational only. 
// Changing this number could have insidious consequences. 
#define TIME_SYSTICK_PERIOD_uS          1000

#define TIME_TICKS_PER_SECOND           (uint32_t)(1000000 / TIME_SYSTICK_PERIOD_uS)
#define TIME_TICKS_PER_MINUTE           (TIME_TICKS_PER_SECOND * 60)
#define TIME_TICKS_PER_HOUR             (TIME_TICKS_PER_MINUTE * 60)
#define TIME_TICKS_PER_DAY              (TIME_TICKS_PER_HOUR * 24)

#define TIME_SECONDS_PER_MINUTE         (uint32_t)(60)
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
 * This (struct) type stores a 'system' time. The 'frac' component is 
 * based on the least count created by the system tick. 
 * 
 * The 'seconds' component is a count of the number of seconds elapsed 
 * since the epoch, defined by tm_epoch, plus the number of leap seconds
 * defined in tm_leapseconds.
 * 
 */
typedef struct TM_SYSTEM_t{
    uint16_t frac;
    uint32_t seconds;
}tm_system_t;

/**
 * @brief System Timedelta Storage Type
 * 
 * This (struct) type stores a 'system' time difference. The 'frac' 
 * component is based on the least count created by the system tick. 
 * 
 */
typedef struct TM_SDELTA_t{
    uint16_t frac;
    uint32_t seconds;
    uint8_t sgn;
}tm_sdelta_t;


/**
 * @brief Real Time Storage Type
 * 
 * This (struct) type stores a 'real' time. This type does not depend on 
 * the epoch and is independent of system time.
 * 
 * This form of time storage is always going to be expensive, and should
 * therefore be used sparingly. 
 * 
 * This type is not time-zone aware, and should always be interpreted as a 
 * UTC time (GMT+0000). See the documentation of time.h for further detail
 * about the interpretation of time by this library.
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
    uint16_t frac; 
}tm_real_t;


/**
 * @brief Real Timedelta Storage Type
 * 
 * This (struct) type stores a 'real' time difference. The 'residual' 
 * component is based on the least count created by the system tick. 
 * 
 * This form of time storage is always going to be expensive, and 
 * should therefore be used sparingly. 
 * 
 */
typedef struct TM_RDELTA_t{
    uint8_t sgn;
    uint16_t frac;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint16_t days;
}tm_rdelta_t;


/**
 * @brief Epoch Change Handler Type
 * 
 * This (struct) type stores an epoch change handler, and must be instantiated
 * and registerd by consumers requiring epoch change information. 
 * 
 */
typedef struct TM_EPOCH_CHANGEHANDLER_t{
    struct TM_EPOCH_CHANGEHANDLER_t * next;
    uint8_t priority;
    void (* func)(tm_sdelta_t *);
}tm_epochchange_handler_t;

/**@}*/ 

/**
 * @name Time Low Level Interface Functions
 * 
 * These functions should be provided by the application/HAL and must be 
 * available at link time.
 */
/**@{*/ 

extern void systick_init(void);

/**@}*/ 

/**
 * @name Time Containers
 * 
 * These containers are defined internally in the library.
 */
/**@{*/ 

extern tm_system_t tm_current;
extern   tm_real_t tm_epoch;
extern      int8_t tm_leapseconds;
extern   tm_real_t tm_internal_epoch;
extern    uint32_t tm_internal_epoch_offset;
extern     uint8_t use_epoch;
extern tm_epochchange_handler_t * epoch_handlers_root;

/**@}*/ 

/**
 * @name Time Initialization and Maintenance Functions
 * 
 */
/**@{*/ 

/**
 * Intitialize time library constructs
 */
uint16_t tm_init(uint16_t ucdm_address);

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
 * @brief Clear a time_system_t instance.
 * 
 * @param stime Pointer to the time_system_t to clear.
 */
void tm_clear_stime(tm_system_t* stime);

/**
 * @brief Clear a time_sdelta_t instance.
 * 
 * @param sdelta Pointer to the time_sdelta_t to clear.
 */
void tm_clear_sdelta(tm_sdelta_t* sdelta);

/**
 * @brief Clear a time_real_t instance.
 * 
 * @param rtime Pointer to the time_real_t to clear.
 */
void tm_clear_rtime(tm_real_t* rtime);

/**
 * @brief Clear a time_delta_t instance.
 * 
 * @param rdelta Pointer to the time_delta_t to clear.
 */
void tm_clear_rdelta(tm_rdelta_t* rdelta);

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
    memcpy((void *)stime, (void *)(&tm_current), sizeof(tm_system_t));
    return;
}

int8_t tm_cmp_stime(tm_system_t * t1, tm_system_t * t2);

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
void tm_get_sdelta(tm_system_t * t1, tm_system_t * t2, tm_sdelta_t * sdelta);


/**
 * @brief Apply a time difference to a time in system time. 
 * 
 * @param t Pointer to (a copy of) the original timestamp. This will be overwritten.
 * @param sdelta Pointer to the time difference to apply 
 */
void tm_apply_sdelta(tm_system_t * t, tm_sdelta_t * sdelta);

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
 * @brief Convert a time_delta_t instance into systick units.
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
 * @param stime The system time (as a pointer to a time_system_t instance)
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
 * of the library uses the Unix Epoch and can handle times upto 2038 AD. 
 * 
 * Variable Epochs will most likely be useful in cases where a second time 
 * reference is maintained relative to the primary Epoch. This type of use
 * is not currently supported by this library, since tm_epoch is a global 
 * which is used by a number of functions. Passing tm_epoch around is going
 * to make it far more expensive than it already is. 
 * 
 * These implementations are here for the sake of completeness. If you 
 * don't actually used the functions, the compiler with get rid of them. 
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
