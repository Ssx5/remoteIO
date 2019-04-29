#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus-version.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "remoteIO_config.h"
#include "remoteIO_modbus.h"
#include "remoteIO_mqtt.h"
int main(int argc, char *argv[])
{
	config_parse();
	modbus_init(&modbus_ctx);
	mqtt_init(&mosq);
	mqtt_start(&mosq);
	modbus_schedule(0);
    return 0;
}

