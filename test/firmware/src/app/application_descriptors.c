

#include "application.h"
#include "application_descriptors.h"
#include <ucdm/descriptor.h>
#include <time/time.h>
#include "sys/modbus/modbus.h"
#include "bsp/hal/uc/id.h"


const descriptor_vendor_t descriptor_vendor = {
    "Quazar Technologies Pvt Ltd",
    "http://www.quazartech.com",
};


const descriptor_hardware_t descriptor_hardware = {
    "HARDWARE_NAME",
    "HARDWARE_VARIANT",
    "HARDWARE_REVISION",
};


const descriptor_firmware_t descriptor_firmware = {
    FIRMWARE_NAME,
    FIRMWARE_VERSION,
};


descriptor_custom_t descriptor_mac = {NULL, 0x00, 8, DESCRIPTOR_ACCTYPE_RFUNC, {&id_read}};


void application_descriptors_init(void){
    descriptor_install(&descriptor_mac);
    modbus_install_descriptor();
    tm_install_descriptor();
}
