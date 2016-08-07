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
 * @file cron.h
 * @brief Cron-like scheduling framework for embebedded systems.
 * 
 * @see cron.c
 */

#ifndef TIME_CRON_H
#define TIME_CRON_H

#include "stddef.h"
#include "time.h"


typedef struct CRON_JOB_t{
    tm_system_t   texec;
    tm_sdelta_t * tafter_p;
    struct CRON_JOB_t * nextjob;
    struct CRON_JOB_t * prevjob;
    void (* handler)(void);
}cron_job_t;

extern cron_job_t * cron_nextjob_p;

void clear_cron_job(cron_job_t* job_p);

void create_cron_job_abs(cron_job_t * job_p, void handler(void), 
                         tm_system_t * texec_p, tm_sdelta_t * tafter_p);

void create_cron_job_rel(cron_job_t * job_p, void handler(void), 
                         tm_sdelta_t * trelexec_p, tm_sdelta_t * tafter_p);

void cron_insert_job(cron_job_t * job_p);

void cron_cancel_job(cron_job_t * job_p);

static inline void cron_replace_job(cron_job_t * job_p);

static inline void cron_replace_job(cron_job_t * job_p){
    cron_cancel_job(job_p);
    cron_insert_job(job_p);
}

void cron_poll(void);

#endif
