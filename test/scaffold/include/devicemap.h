/**
 * @file devicemap.h
 * @brief Map of device configuration and data access parameters.
 *
 * This file, and the file hierarchy that it includes, if any, should 
 * define all the configuration and data access parameters that the 
 * device exposes via the ucdm interface. 
 * 
 * All source files operating on the parameters in any way must include 
 * this file and use the appropriate define to perform it's operation at 
 * the correct address within the device map. External interfaces should
 * use the functions of `libucdm` to ensure correct access.
 * 
 * Any lower level files in this hierarchy should not be included 
 * separately, and the information present in them should be accessed
 * only though this file.
 * 
 * @see `libucdm`
 */

#ifndef DEVICEMAP_H
#define DEVICEMAP_H

#include <ucdm/ucdm.h>

#define DMAP_MAX_REGISTERS              255
#define DMAP_MODBUS_BASE_ADDRESS        0
#define DMAP_TIME_BASE_ADDRESS          2
#endif
