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
 * @file time.c
 * @brief Time related function implementations.
 * 
 * See time.h for usage documentation. Implementation 
 * documentation should be added to this file as some point. 
 * 
 * @see time.h
 */

#include <ds/sllist.h>
#include <ucdm/ucdm.h>
#include <ucdm/descriptor.h>
#include <string.h>
#include "time.h"
#include "systick.h"
#include "sync.h"
#include "cron.h"


volatile tm_system_t tm_current = 0;
tm_real_t tm_epoch =          {19, 70, 1, 1, 0, 0, 0, 0};
tm_real_t tm_internal_epoch = {19, 69, 3, 1, 0, 0, 0, 0};
int64_t tm_internal_epoch_offset =  26438400 * 1000LL;
uint8_t use_epoch = 1;
int8_t tm_leapseconds = 0;

tm_epochchange_handler_t * epoch_handlers_root = NULL;

/** @brief Time Library Epoch Descriptor */
descriptor_custom_t tm_epoch_descriptor = {NULL, DESCRIPTOR_TAG_TIME_EPOCH, 
    sizeof(tm_real_t), DESCRIPTOR_ACCTYPE_PTR, {&tm_epoch}};

/** @brief Time Library Version Descriptor */
static descriptor_custom_t time_descriptor = {NULL, DESCRIPTOR_TAG_LIBVERSION,
    sizeof(TIME_VERSION), DESCRIPTOR_ACCTYPE_PTR, {TIME_VERSION}};

    
void tm_install_descriptor(void)
{
    descriptor_install(&time_descriptor);
}    

#define TM_UCDM_STIME_LEN     (sizeof(tm_system_t) / 2 + (sizeof(tm_system_t) % 2 != 0))
#define TM_UCDM_RTIME_LEN     (sizeof(tm_real_t)   / 2 + (sizeof(tm_real_t)   % 2 != 0))

uint16_t tm_init(uint16_t ucdm_address){
    tm_current = 0;

    #if TIME_EXPOSE_UCDM
    for (uint8_t i=0; i < TM_UCDM_STIME_LEN; i ++, ucdm_address++){
        ucdm_redirect_regr_ptr(ucdm_address, 
                               ((uint16_t *)(void *)(&tm_current) + i));
    }
    descriptor_install(&tm_epoch_descriptor);
    #endif

    #if TIME_ENABLE_SYNC
    ucdm_address = tm_sync_init(ucdm_address);
    #endif

    #if TIME_ENABLE_CRON
    tm_cron_init();
    #endif

    tm_systick_init();
    return ucdm_address;
}

void tm_clear_stime(tm_system_t* stime){
    *stime = 0;
    return;
}

void tm_clear_sdelta(tm_sdelta_t* sdelta){
    *sdelta = 0;
    return;
}

void tm_clear_rtime(tm_real_t* rtime){
    rtime->millis = 0;
    rtime->seconds = 0;
    rtime->minutes = 0;
    rtime->hours = 0;
    rtime->date = 0;
    rtime->month = 0;
    rtime->year = 0;
    rtime->century = 0;
    return;
}

void tm_clear_rdelta(tm_rdelta_t* rdelta){
    rdelta->days = 0;
    rdelta->hours = 0;
    rdelta->minutes = 0;
    rdelta->seconds = 0;
    rdelta->millis = 0;
    rdelta->sgn = 0;
    return;
}

uint8_t tm_check_invalid_rtime(tm_real_t* rtime){
    if (rtime->century > 24) {
        return 1;
    } 
    if (rtime->year > 99) {
        return 2;
    }
    if (rtime->month < 1 || rtime->month > 12) {
        return 3;
    }
    if (rtime->date < 1 || rtime->date > 31) {
        // This should actually check based on month 
        // and year
        return 4;
    }
    if (rtime->hours > 23) {
        return 5;
    }
    if (rtime->minutes > 59) {
        return 6;
    }
    if (rtime->seconds > 59) {
        return 7;
    }
    if (rtime->millis > 999) {
        return 8;
    }
    return 0;
}


void tm_sdelta_from_rdelta(tm_rdelta_t* rdelta, tm_sdelta_t* sdelta){
    uint64_t result;
    result  = rdelta->days    * TIME_SECONDS_PER_DAY * 1000LL;
    result += rdelta->hours   * TIME_SECONDS_PER_HOUR * 1000LL;
    result += rdelta->minutes * TIME_SECONDS_PER_MINUTE * 1000LL;
    result += rdelta->seconds * 1000LL;
    result += rdelta->millis;
    if (rdelta->sgn) {
        result = result * -1;
    }
    *sdelta = result;
}

void tm_rdelta_from_sdelta(tm_sdelta_t* sdelta, tm_rdelta_t* rdelta){
    tm_clear_rdelta(rdelta);
    tm_sdelta_t sdelta_copy = *sdelta;
    
    if (sdelta_copy < 0){
        rdelta->sgn = 1; 
        sdelta_copy *= -1;
    }

    rdelta->days = sdelta_copy / (TIME_SECONDS_PER_DAY * 1000LL);
    sdelta_copy -= rdelta->days * (TIME_SECONDS_PER_DAY * 1000LL);

    rdelta->hours = sdelta_copy / (TIME_SECONDS_PER_HOUR * 1000LL);
    sdelta_copy -= rdelta->hours * (TIME_SECONDS_PER_HOUR * 1000LL);

    rdelta->minutes = sdelta_copy / (TIME_SECONDS_PER_MINUTE * 1000LL);
    sdelta_copy -= rdelta->minutes * (TIME_SECONDS_PER_MINUTE * 1000LL);
    
    rdelta->seconds = sdelta_copy / 1000LL;
    sdelta_copy -= rdelta->seconds * 1000LL;
    
    rdelta->millis = sdelta_copy;
    return;
}

static const int16_t days_to_month[]=
    {0, 306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

static const int8_t days_in_month[]=
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint16_t gyear(tm_real_t * rtime);

static uint16_t gyear(tm_real_t * rtime){
    uint16_t result = (uint16_t)(rtime->century) * 100 + rtime->year;
    return (result);
}

void tm_stime_from_rtime(tm_real_t* rtime, tm_system_t * stime){
    int32_t days = rtime->date - 1;
    uint16_t syear = 0;
    int16_t years = 0;
    
    if (!use_epoch){
        return;
    }
    
    days += days_to_month[rtime->month];
    syear = gyear(&tm_internal_epoch);
    years = (int16_t)(gyear(rtime) - syear);
    if (rtime->month < 3){
        years --;
    } else {
        uint16_t lyear = gyear(rtime);
        if ((!(lyear % 4) && (lyear % 100)) || !(lyear % 400)){
            days ++;
        }
    }

    days += (years * 365);

    while (years > 0){
        if ((!(syear % 4) && (syear % 100)) || !(syear % 400)){
            days ++;
        }
        syear ++;
        years --;
    }
    while (years < 0){
        if ((!(syear % 4) && (syear % 100)) || !(syear % 400)){
            days --;
        }
        syear --;
        years ++;
    }

    *stime = ((          days * TIME_SECONDS_PER_DAY * 1000LL)    +
              (  rtime->hours * TIME_SECONDS_PER_HOUR * 1000LL)   +
              (rtime->minutes * TIME_SECONDS_PER_MINUTE * 1000LL) + 
              (rtime->seconds * 1000LL) + rtime->millis + 
              tm_leapseconds - tm_internal_epoch_offset);
}


void tm_rtime_from_stime(tm_system_t* stime, tm_real_t* rtime){
    if (!use_epoch){
        return;
    }

    *rtime = tm_internal_epoch;
    tm_system_t stime_internal = *stime + tm_internal_epoch_offset - tm_leapseconds;
    uint32_t days = stime_internal / (TIME_SECONDS_PER_DAY * 1000);
    uint32_t remaining_ms = stime_internal % (TIME_SECONDS_PER_DAY * 1000);

    rtime->hours = remaining_ms / (TIME_SECONDS_PER_HOUR * 1000);
    remaining_ms %= (TIME_SECONDS_PER_HOUR * 1000);
    rtime->minutes = remaining_ms / (TIME_SECONDS_PER_MINUTE * 1000);
    remaining_ms %= (TIME_SECONDS_PER_MINUTE * 1000);
    rtime->seconds = remaining_ms / 1000;
    rtime->millis = remaining_ms % 1000;

    uint16_t year = gyear(&tm_internal_epoch);
    while (days >= 365) {

        uint16_t leap_adjustment = ((!((year+1)% 4) && ((year+1) % 100)) || !((year+1) % 400)) ? 366 : 365;
        if (days < leap_adjustment) break;
        days -= leap_adjustment;
        year++;
    }
    
    // Calculate month, starting from march
    uint8_t next_month = 3;
    uint8_t next_month_days = 31;
    while (days >= next_month_days) {
        days -= next_month_days;
        if (next_month == 12) {
            year ++;
            next_month = 1;
        } else {
            next_month ++;
        }
        rtime->month = next_month;
                
        if (next_month == 2 && ((!(year % 4) && (year % 100)) || !(year % 400))){
            next_month_days = 29;
        } else {
            next_month_days = days_in_month[next_month];
        }
    }
    rtime->date = days + 1; // Days are 1-indexed 
    rtime->year = year % 100;
    rtime->century = year / 100;
}

/*
 * The internal epoch is used to allow arbitrary epoch setting. 
 * All our calculations related to rtime conversions are based on 
 * an epoch at midnight, the 1st of March. When another epoch is 
 * set, we choose a point just before the chosen epoch which has  
 * the needed characteristics as the internal epoch. We also 
 * maintain the offset between the actual epoch and the internal 
 * epoch. 
 * 
 * Timestamps in the system are all stored against the 
 * configured epoch. The internal epoch is only used ephemerally 
 * while converting between the tm_system_t and tm_real_t.
 */
static void tm_set_internal_epoch(tm_real_t* rtime);

static void tm_set_internal_epoch(tm_real_t* rtime){
    tm_internal_epoch.millis = 0;
    tm_internal_epoch.seconds = 0;
    tm_internal_epoch.minutes = 0;
    tm_internal_epoch.hours = 0;
    tm_internal_epoch.date = 1;
    tm_internal_epoch.month = 3;
    if (rtime->month >= 3){
        tm_internal_epoch.year = rtime->year;
        tm_internal_epoch.century = rtime->century;
    }
    else{
        if (rtime->year){
            tm_internal_epoch.year = rtime->year - 1;
            tm_internal_epoch.century = rtime->century;
        }
        else{
            tm_internal_epoch.year = 99;
            tm_internal_epoch.century = rtime->century - 1;
        }
    }
    uint16_t days = rtime->date - 1;
    days += days_to_month[rtime->month];
    tm_internal_epoch_offset = ((          days * TIME_SECONDS_PER_DAY    ) +
                                (  rtime->hours * TIME_SECONDS_PER_HOUR   ) +
                                (rtime->minutes * TIME_SECONDS_PER_MINUTE ) + 
                                (rtime->seconds                           )) * 1000 ;
    return;
}

void tm_set_epoch(tm_real_t* rtime, uint8_t follow){
    tm_sdelta_t sdelta;
    tm_system_t original_epoch;
    tm_system_t new_epoch;

    
    if (follow){
        tm_stime_from_rtime(&tm_epoch, &original_epoch);
        tm_stime_from_rtime(rtime, &new_epoch);
        tm_get_sdelta(&original_epoch, &new_epoch, &sdelta);
        critical_enter();
        tm_apply_sdelta((tm_system_t *)&tm_current, &sdelta);
    }
    else{
        tm_clear_sdelta(&sdelta);
        critical_enter();
        tm_clear_stime((tm_system_t *)&tm_current);
        tm_current = tm_internal_epoch_offset;
    }

    memcpy((void*)(&tm_epoch), (void*)rtime, sizeof(tm_real_t));
    tm_set_internal_epoch(rtime);
    
    use_epoch = 1;
    
    tm_epochchange_handler_t * echandler = epoch_handlers_root;
    while(echandler){
        echandler -> func(&sdelta);
        echandler = echandler->next;
    }

    critical_exit();
    return;
}

void tm_register_epoch_change_handler(tm_epochchange_handler_t * handler){
    sllist_install((void *)&epoch_handlers_root, (void *)handler);
}
