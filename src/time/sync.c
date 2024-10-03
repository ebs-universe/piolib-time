/*
 *    Copyright (c) 
 *      (c) 2016-2018 Chintalagiri Shashank, Firefly Aerospace Pvt.Ltd.
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
 * @file sync.c
 * @brief Time synchonization function implementation
 * 
 * Time synchronization is implemented as a state machine. The time 
 * synchronization state is stored in tm_sync_sm, and contains the 
 * state as a uint8_t and the following timestamps as tm_system_t: 
 *  - t1  - first sync timestamp provided by the master
 *  - t1p - local time on receipt of the first sync timestamp
 *  - t2  - timestamp read by the master for delay calculation
 *  - t2p - master timestamped reception of t2
 * 
 * The state machine is executed on reciept of the last synchronization 
 * register from the host. 
 * 
 * When the delay out timestamp is read from the host (t2), it needs to 
 * read one extra register. The final register read is the trigger we 
 * use to record our copy.
 * 
 * 
 * @see sync.h
 */


#include <ucdm/ucdm.h>
#include "time.h"
#include "sync.h"

#if TIME_ENABLE_SYNC

avlt_node_t  tm_avlt_sync_handler_node;
tm_sync_sm_t tm_sync_sm;
uint16_t tm_sync_host_read_hook(uint16_t address);


uint16_t tm_sync_init(uint16_t ucdm_address){
    // Setup System Time Read Hook
    ucdm_redirect_regr_func(ucdm_address, &tm_sync_host_read_hook);
    ucdm_address += 2;
    
    // Setup Host Interface Registers
    for(uint8_t i=0; i<(sizeof(tm_system_t)/2); i++, ucdm_address++){
        ucdm_enable_regw(ucdm_address + i);        
    }
    
    // Setup Host Sync Handler(s)
    ucdm_install_regw_handler(ucdm_address - 1, 
                              &tm_avlt_sync_handler_node, 
                              &tm_sync_handler);
    tm_sync_sm.state = TM_SYNC_STATE_IDLE;
    return ucdm_address;
}

void tm_sync_request_host(void){
    ucdm_exception_status |= UCDM_EXST_TIMESYNC_REQ;
    tm_sync_sm.state = TM_SYNC_STATE_WAIT_HOST;
}


static inline void tm_sync_apply(void);

static inline void tm_sync_apply(void){
    tm_sdelta_t offset, tsd1, tsd2;

    tm_get_sdelta(&(tm_sync_sm.t1), &(tm_sync_sm.t1p), &(tsd1));
    tm_get_sdelta(&(tm_sync_sm.t2), &(tm_sync_sm.t2p), &(tsd2));
    
    offset = (tsd1 - tsd2)/ 2;
    
    critical_enter();
    tm_apply_sdelta((tm_system_t *)&tm_current, &offset);
    critical_exit();

    tm_epochchange_handler_t * echandler = epoch_handlers_root;
    while(echandler){
        echandler -> func(&offset);
        echandler = echandler->next;
    }
}

uint16_t tm_sync_host_read_hook(uint16_t address){
    if (tm_sync_sm.state == TM_SYNC_STATE_WAIT_DELAY_OUT){
        tm_sync_sm.state = TM_SYNC_STATE_WAIT_DELAY_IN;
        // Host read our timestamp for delay calculation.
        // Store the timestamp sent out
        tm_current_time(&(tm_sync_sm.t2));
    }
    return 0;
}

void tm_sync_handler(uint16_t addr){
    switch(tm_sync_sm.state){
        case TM_SYNC_STATE_PREINIT:
            break;
        case TM_SYNC_STATE_IDLE:
            if (ucdm_register[addr].data){
                // Got sync with timestamp from the host. 
                tm_sync_sm.state = TM_SYNC_STATE_WAIT_DELAY_OUT;
                memcpy(
                    &tm_sync_sm.t1, 
                    (void *)&ucdm_register[addr - (sizeof(tm_system_t) / 2) + 1], 
                    sizeof(tm_system_t)
                );
            } else { 
                // Got sync without timestamp from the host. 
                tm_sync_sm.state = TM_SYNC_STATE_WAIT_FOLLOW_UP;
            }
            tm_current_time(&(tm_sync_sm.t1p));
            break;
        case TM_SYNC_STATE_WAIT_FOLLOW_UP:
            // Got the sync timestamp from the host. 
            tm_sync_sm.state = TM_SYNC_STATE_WAIT_DELAY_OUT;
            memcpy(
                &tm_sync_sm.t1, 
                (void *)&ucdm_register[addr - (sizeof(tm_system_t) / 2) + 1], 
                sizeof(tm_system_t)
            );
            break;
        case TM_SYNC_STATE_WAIT_DELAY_OUT:
            // This is handled in the register read function for the 
            // system time register of UCDM. t2 will be set there. 
            break;
        case TM_SYNC_STATE_WAIT_DELAY_IN:
            // Host returned its timestamp for delay calculation.
            memcpy(
                &tm_sync_sm.t2p, 
                (void *)&ucdm_register[addr - (sizeof(tm_system_t) / 2) + 1], 
                sizeof(tm_system_t)
            );
            tm_sync_sm.state = TM_SYNC_STATE_IDLE;
            // All information is now available. Calculate and apply.
            tm_sync_apply();
            ucdm_exception_status &= ~UCDM_EXST_TIMESYNC_REQ;
            break;
        default:
            break;
    }
    return;
}

#endif