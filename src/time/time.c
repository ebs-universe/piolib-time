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


volatile tm_system_t tm_current = {0, 0};
tm_real_t tm_epoch =          {19, 70, 1, 1, 0, 0, 0, 0};
tm_real_t tm_internal_epoch = {19, 69, 3, 1, 0, 0, 0, 0};
uint32_t tm_internal_epoch_offset =  26438400;
uint8_t use_epoch = 1;
int8_t tm_leapseconds = 0;

tm_epochchange_handler_t * epoch_handlers_root = NULL;

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
#define TM_UCDM_RTIME_LEN     (sizeof(tm_real_t)   / 2 + (sizeof(tm_real_t) % 2 != 0))

uint16_t tm_init(uint16_t ucdm_address){
    tm_current.seconds = 0;
    tm_current.frac = 0;
    for (uint8_t i=0; i < TM_UCDM_STIME_LEN; i ++, ucdm_address++){
        ucdm_redirect_regr_ptr(ucdm_address, 
                               ((uint16_t *)(void *)(&tm_current) + i));
    }
    descriptor_install(&tm_epoch_descriptor);
    tm_systick_init();
    return ucdm_address;
}

void tm_clear_stime(tm_system_t* stime){
    stime->seconds = 0;
    stime->frac = 0;
    return;
}

void tm_clear_sdelta(tm_sdelta_t* sdelta){
    sdelta->seconds = 0;
    sdelta->frac = 0;
    sdelta->sgn = 0;
    return;
}

void tm_clear_rtime(tm_real_t* rtime){
    rtime->frac = 0;
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
    rdelta->frac = 0;
    rdelta->sgn = 0;
    return;
}

int8_t tm_cmp_stime(tm_system_t * t1, tm_system_t * t2){
    if (t1->seconds < t2->seconds){
        return (-1);
    }
    else if (t1->seconds > t2->seconds){
        return (1);
    }
    else if (t1->frac < t2->frac){
        return (-1);
    }
    else if (t1->frac > t2->frac){
        return (1);
    }
    else{
        return 0;
    }
}

void tm_get_sdelta(tm_system_t * t1, tm_system_t * t2, tm_sdelta_t * sdelta){
    uint8_t cfrac = 0;
    if (t2->frac > t1->frac){
        cfrac = 0;
        sdelta->frac = t2->frac - t1->frac;
    }
    else{
        cfrac = 1;
        sdelta->frac = t1->frac - t2->frac;
    }
    
    if (t2->seconds == t1->seconds){
        sdelta->seconds = 0; 
        sdelta->sgn = cfrac;
    }
    else if (t2->seconds > t1->seconds){
        sdelta->sgn = 0;
        sdelta->seconds = t2->seconds - t1->seconds - cfrac;
    }
    else{
        sdelta->sgn = 1;
        sdelta->seconds = t1->seconds - t2->seconds + cfrac - 1;
    }
}

void tm_apply_sdelta(tm_system_t * t, tm_sdelta_t * sdelta){
    if (sdelta->sgn == 0){
        t->seconds += sdelta->seconds;
        t->frac += sdelta->frac;
        if (t->frac >= TIME_TICKS_PER_SECOND){
            t->frac -= TIME_TICKS_PER_SECOND;
            t->seconds += 1;
        }
    }
    else{
        t->seconds -= sdelta->seconds;
        if (t->frac < sdelta->frac){
            t->frac = TIME_TICKS_PER_SECOND - (sdelta->frac - t->frac);
            t->seconds -= 1;
        }
        else{
            t->frac -= sdelta->frac;
        }
    }
    return;
}

void tm_sdelta_from_rdelta(tm_rdelta_t* rdelta, tm_sdelta_t* sdelta){
    sdelta->seconds  = rdelta->days    * TIME_SECONDS_PER_DAY;
    sdelta->seconds += rdelta->hours   * TIME_SECONDS_PER_HOUR;
    sdelta->seconds += rdelta->minutes * TIME_SECONDS_PER_MINUTE;
    sdelta->seconds += rdelta->seconds;
    sdelta->frac = rdelta->frac;
    sdelta->sgn = rdelta->sgn;
    return;
}

void tm_rdelta_from_sdelta(tm_sdelta_t* sdelta, tm_rdelta_t* rdelta){
    tm_clear_rdelta(rdelta);
    
    rdelta->sgn = sdelta->sgn;
    
    rdelta->days = sdelta->seconds / (TIME_SECONDS_PER_DAY);
    sdelta->seconds -= rdelta->days * TIME_SECONDS_PER_DAY;
    
    rdelta->hours = sdelta->seconds / (TIME_SECONDS_PER_HOUR);
    sdelta->seconds -= rdelta->hours * TIME_SECONDS_PER_HOUR;
    
    rdelta->minutes = sdelta->seconds / (TIME_SECONDS_PER_MINUTE);
    sdelta->seconds -= rdelta->minutes * TIME_SECONDS_PER_MINUTE;
    
    rdelta->seconds = sdelta->seconds;
    rdelta->frac = sdelta->frac;
    return;
}

static const int16_t days_to_month[]=
    {0, 306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

static uint16_t gyear(tm_real_t * rtime);

static uint16_t gyear(tm_real_t * rtime){
    return ((uint16_t)(rtime->century) * 100 + rtime->year);
}

void tm_stime_from_rtime(tm_real_t* rtime, tm_system_t * stime){
    uint16_t days = rtime->date - 1;
    uint16_t syear = 0;
    uint8_t years = 0;
    
    if (!use_epoch){
        return;
    }
    stime->frac = rtime->frac;
    
    days += days_to_month[rtime->month];
    syear = gyear(&tm_internal_epoch);
    years = (uint8_t)(gyear(rtime) - syear);
    if (rtime->month < 3){
        years --;
    }
    days += (years * 365);
    
    while (years){
        if ((!(syear % 4) && (syear % 100)) || !(syear % 400)){
            days ++;
        }
        syear ++;
        years --;
    }
    
    stime->seconds = ((          days * TIME_SECONDS_PER_DAY    )+
                      (  rtime->hours * TIME_SECONDS_PER_HOUR   )+
                      (rtime->minutes * TIME_SECONDS_PER_MINUTE )+ 
                      (rtime->seconds) + (tm_leapseconds)       );
    stime->seconds -= tm_internal_epoch_offset;
    return;
}

void tm_rtime_from_stime(tm_system_t* stime, tm_real_t* rtime){
    if (!use_epoch){
        return;
    }
}

static void tm_set_internal_epoch(tm_real_t* rtime);

static void tm_set_internal_epoch(tm_real_t* rtime){
    tm_internal_epoch.frac = 0;
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
    tm_internal_epoch_offset = ((          days * TIME_SECONDS_PER_DAY    )+
                                (  rtime->hours * TIME_SECONDS_PER_HOUR   )+
                                (rtime->minutes * TIME_SECONDS_PER_MINUTE )+ 
                                (rtime->seconds                           ));
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
    }
    else{
        tm_clear_sdelta(&sdelta);
    }
    memcpy((void*)(&tm_epoch), (void*)rtime, sizeof(tm_real_t));
    tm_set_internal_epoch(rtime);
    // TODO This seems quite incomplete! It may have been left uncompleted at 
    // the last major rewrite. The current time seems to be lost entirely, and 
    // set instead to the epoch point itself. 
    tm_clear_stime(&tm_current);
    tm_current.seconds = tm_internal_epoch_offset;
    use_epoch = 1;
    
    tm_epochchange_handler_t * echandler = epoch_handlers_root;
    while(echandler){
        echandler -> func(&sdelta);
        echandler = echandler->next;
    }
    return;
}

void tm_register_epoch_change_handler(tm_epochchange_handler_t * handler){
    sllist_install((void *)&epoch_handlers_root, (void *)handler);
}
