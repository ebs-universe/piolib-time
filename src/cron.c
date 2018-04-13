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
 * @file cron.c
 * @brief Cron-like scheduling framework implementations.
 * 
 * @see cron.h
 */

#include "cron.h"

cron_job_t * cron_nextjob_p = NULL;

tm_epochchange_handler_t cron_change_handler = {NULL, 3, &tm_cron_epoch_change_handler};


void tm_cron_init(void){
    tm_register_epoch_change_handler(&cron_change_handler);
}


void tm_cron_clear_job(cron_job_t * job_p){
    tm_cron_cancel_job(job_p);
    job_p->handler = NULL;
    job_p->nextjob = NULL;
    job_p->prevjob = NULL;
    job_p->tafter_p = NULL;
    job_p->texec.seconds = 0;
    job_p->texec.frac = 0;
}


void tm_cron_create_job_abs(cron_job_t * job_p, void handler(void), 
                         tm_system_t * texec_p, tm_sdelta_t * tafter_p){
    job_p->handler = handler;
    job_p->tafter_p = tafter_p;
    job_p->texec.seconds = texec_p->seconds;
    job_p->texec.frac = texec_p->frac;
    tm_cron_insert_job(job_p);
    return;
}


void tm_cron_create_job_rel(cron_job_t * job_p, void handler(void), 
                         tm_sdelta_t * trelexec_p, tm_sdelta_t * tafter_p){
    job_p->handler = handler;
    job_p->tafter_p = tafter_p;
    tm_current_time(&(job_p->texec));
    tm_apply_sdelta(&(job_p->texec), trelexec_p);
    tm_cron_insert_job(job_p);
    return;
}


void tm_cron_insert_job(cron_job_t * job_p){
    cron_job_t * walker = cron_nextjob_p;
    if (!walker){
        // There are no jobs to start with.
        cron_nextjob_p = job_p;
        return;
    }
    while(walker){
        if (!(walker->nextjob)){
            // This is the last job.
            walker->nextjob = job_p;
            return;
        }
        else if (tm_cmp_stime(&(job_p->texec), &(walker->texec)) > 0){
            // job_p needs to be inserted before walker. 
            job_p->nextjob = walker;
            job_p->prevjob = walker->prevjob;
            walker->prevjob = job_p;
            if (job_p->prevjob){
                // walker is not the first job
                job_p->prevjob->nextjob = job_p;
                return;
            }
            else{
                // walker is the first job
                cron_nextjob_p = job_p;
                return;
            }
        }
    }
}


void tm_cron_cancel_job(cron_job_t * job_p){
    if (job_p->nextjob){
        job_p->nextjob->prevjob = job_p->prevjob;
    }
    if (job_p->prevjob){
        job_p->prevjob->nextjob = job_p->nextjob;
    }
    else{
        cron_nextjob_p = job_p->nextjob;
        cron_nextjob_p->prevjob = NULL;
    }
}


void tm_cron_poll(void){
    if (!cron_nextjob_p){
        return;
    }
    if (tm_cmp_stime(&(cron_nextjob_p->texec), &tm_current) >= 0){
        cron_nextjob_p->handler();
        if (cron_nextjob_p->tafter_p){
            tm_apply_sdelta(&(cron_nextjob_p->texec), cron_nextjob_p->tafter_p);
            tm_cron_replace_job(cron_nextjob_p);
        }
        else{
            cron_nextjob_p = cron_nextjob_p->nextjob;
            cron_nextjob_p->prevjob = NULL;
        }
    }
}

void tm_cron_epoch_change_handler(tm_sdelta_t * offset){
    cron_job_t * walker = cron_nextjob_p;
    while (walker){
        tm_apply_sdelta(&(walker->texec), offset);
        walker = walker->nextjob;
    }
}
