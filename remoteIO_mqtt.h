#ifndef __REMOTEIO_MQTT_H__
#define __REMOTEIO_MQTT_H__

#include <mosquitto.h>

extern struct mosquitto *mosq;

int mqtt_init(struct mosquitto **mosq);
void mqtt_start(struct mosquitto **mosq);
#endif