/*
 *    Copyright (c) 
 *      (c) 2016      Chintalagiri Shashank, Firefly Aerospace Pvt.Ltd.
 *      (c) 2010-2011 Chintalagiri Shashank, Jugnu, IIT Kanpur
 * 
 *    This file is part of
 *    Embedded bootstraps : time library
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 * 
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/**
 * @file sync.h
 * @brief Time synchonization functions for embebedded systems.
 * 
 * The Time Synchronization implementation presented here is based on the 
 * synchronization elements of the IEEE 1588 Precision Time Protocol. The 
 * bulk of the IEEE 1588 specification is ignored to provide a bare 
 * minimum time synchronization implementation suitable for a reasonable 
 * degree of time synchronization (not high precision) on highly constrained
 * platforms. 
 * 
 * This implementation neither knows nor cares about the specifics of the 
 * transport used. Synchonization precision will be highly dependent on the
 * stability and symmetry of the transport used. Due to its transport 
 * independent design, this implementation cannot take advantage of 
 * specialized PHYs intended for precision time synchronization such as 
 * PHYTER.
 * 
 * This implementation assumes that the system is able to propagate and 
 * maintain it's own time, and does so with the implementation included
 * in this library (or one compatible with it). 
 * 
 * This implementation also assumes that the following pieces of information 
 * are known to both the host and the device : 
 *   - epoch, as in `tm_epoch`
 *   - resolution of the frac, as in `TIME_SYSTICKS_PER_SECOND`
 * 
 * Applications enabling this functionality must ensure TIME_ENABLE_SYNC 
 * is defined and non-zero. TM_BASE_ADDRESS as provided to `tm_init` should 
 * be known. 
 * 
 * The following is the general synchronization process host-side 
 * applications must implement to use this interface: 
 *   - master must initiate the synchronization process by writing a 
 *     timestamp to the appropriate UCDM registers.UCDM
 * 
 * Utilization of this library is typically in one of three ways :
 * 
 *   - Time Initialization
 *   - Ephemeral Time Synchronization
 *   - Continuous Time Synchronization
 * 
 * Each approach is more complex than the last, and involves more system 
 * resources.
 *
 * 1. Time Initialization
 * 
 *   This is the simplest means of utilizing this implementation. This kind 
 *   of application requires the assumption that the system is perfectly
 *   capable of maintaining it's own clocks, once intialized to the correct
 *   time. This assumption is usually invalidated over long periods due to 
 *   clock drift, and over even longer periods (years) due to leap seconds.
 * 
 * 2. Ephemeral Time Synchronization
 * 
 *   This is likely the optimal use of this implementation. In this kind of 
 *   application, time is synchonized at specific 'safe' points of the 
 *   application. In order to use this implementation, the host must know
 *   when the device is safely able to synchonize. This synchronization 
 *   will hold until the next sychronization happens, after which the system
 *   is more or less reset to a known temporal state. At the next 
 *   synchronization, any stored timstamps within the application should 
 *   effectively be invalidated, or be offset by a degree equal to that of 
 *   the offset synchonized. 
 * 
 * 3. Continuous Time Synchonization
 * 
 *   This is the most complicated use of this implementation, with serious 
 *   implications to system performance. When time is synchonized, this 
 *   implementation will pass along the offset to all subsystems which have 
 *   stored timestamps, and all of them must apply these offsets to all 
 *   timestamps they store which require this precision. This kind of 
 *   synchonization is likely to have dangerous consequences to scheduled 
 *   events and temporally marked data streams. 
 * 
 * @see sync.c
 */

#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#define TM_SYNC_STATE_PREINIT        0
#define TM_SYNC_STATE_IDLE           1   
#define TM_SYNC_STATE_WAIT_HOST      2
#define TM_SYNC_STATE_WAIT_FOLLOW_UP 3
#define TM_SYNC_STATE_WAIT_DELAY_IN  4
#define TM_SYNC_STATE_WAIT_DELAY_OUT 5

#include <ds/avltree.h>
#include "time.h"

typedef struct TM_SYNC_SM_t{
    uint8_t state;
    tm_system_t t1;
    tm_system_t t1p;
    tm_system_t t2;
    tm_system_t t2p;
} tm_sync_sm_t;

#if TIME_ENABLE_SYNC
ucdm_addr_t tm_sync_init(ucdm_addr_t ucdm_next_address);
void tm_sync_request_host(void);
void tm_sync_handler(ucdm_addr_t addr);
extern avlt_node_t  tm_avlt_sync_handler_node;
#endif

#if TIME_ENABLE_SYNC_RTC
uint8_t tm_sync_current_to_rtc(void);
uint8_t tm_sync_current_from_rtc(void);
#endif

#endif

