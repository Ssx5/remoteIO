#ifndef __REMOTEIO_MODBUS_H__
#define __REMOTEIO_MODBUS_H__

#include <modbus/modbus.h>



extern modbus_t *modbus_ctx;



int modbus_init(modbus_t ** ctx);
void* modbus_schedule(void * arg);

#endif