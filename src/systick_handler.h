/*
    Copyright (c) 
      (c) 2016      Chintalagiri Shashank, Firefly Aerospace Pvt.Ltd.

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
 * @file systick_handler.h
 * @brief Interrupt handler for the system tick for static inclusion 
 *        in the application. 
 * 
 */

#include "time.h"


static inline void time_systick_handler(void);

static inline void time_systick_handler(void){
    if (tm_current.frac == TIME_TICKS_PER_SECOND -1){
        tm_current.seconds ++;
        tm_current.frac = 0;
    }
    else{
        tm_current.frac ++;
    }
}