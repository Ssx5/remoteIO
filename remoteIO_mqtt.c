#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "remoteIO_mqtt.h"
#include "remoteIO_config.h"
#include "remoteIO_modbus.h"
#include <errno.h>
struct mosquitto *mosq;

void publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
    printf("Message has been sent!");
}


void connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i,ret;
    if (!result)
    {
        /* Subscribe to broker information topics on successful connect. */
        //mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
        printf("connect successful!\n");
        for (int i = 0; i < rio_cfg.n_resources; ++i)
        {
            struct operation_set_t *set = rio_cfg.resources[i].set;
            if (set)
            {
                mosquitto_subscribe(mosq, NULL, set->topic, set->qos);
            }
        }
    }
    else
    {
        fprintf(stderr, "Connect failed: %d\n", errno);
        exit(0);
    }
}

void subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;
	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}


void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
#ifdef DEBUG
    printf("Recv len: %d, message: %s\n", message->payloadlen, (char *)message->payload);
#endif
    if (message->payloadlen != 1)
        return;
    int state;
    if (((char *)message->payload)[0] == '0' || ((char *)message->payload)[0] == 0)
        state = 0;
    else
        state = 1;
    char *topic = message->topic;
    for (int i = 0; i < rio_cfg.n_resources; ++i)
    {
        struct operation_set_t *set = rio_cfg.resources[i].set;
        if (set && strcmp(topic, set->topic) == 0)
        {
            switch (set->func_code)
            {
            case 5:
                modbus_write_bit(modbus_ctx, set->start_addr, state);
#ifdef DEBUG
                printf("modbus_write_bit(%d, %d)\n", set->start_addr, state);
#endif
                break;
            default:
                break;
            }
        }
    }
}

int mqtt_init(struct mosquitto **mosq)
{
    int ret;
    mosquitto_lib_init();

    int major, minor, revision;
    mosquitto_lib_version(&major, &minor, &revision);
    printf("libmosquitto: major: %d, minor: %d, revision: %d\n", major, minor, revision);

    *mosq = mosquitto_new(rio_cfg.mqtt_server.client_id, rio_cfg.mqtt_server.clean_session, NULL);

    if (!mosq)
    {
        fprintf(stderr, "mosquitto_new() error\n");
        exit(0);
    }

#ifdef DEBUG
    printf("mosquitto_new(%s, %d, NULL)\n", rio_cfg.mqtt_server.client_id, rio_cfg.mqtt_server.clean_session);
#endif

    if (strlen(rio_cfg.mqtt_server.username) > 0 || strlen(rio_cfg.mqtt_server.password) > 0)
        mosquitto_username_pw_set(*mosq, rio_cfg.mqtt_server.username, rio_cfg.mqtt_server.password);

    if (rio_cfg.mqtt_server.tls_enable)
    {
        ret = mosquitto_tls_set(*mosq, rio_cfg.mqtt_server.cafile,
                                rio_cfg.mqtt_server.capath,
                                rio_cfg.mqtt_server.certfile,
                                rio_cfg.mqtt_server.keyfile,
                                NULL);
#ifdef DEBUG
        printf("mosquitto_tls_set(mosq, %s, %s)\n", rio_cfg.mqtt_server.cafile, rio_cfg.mqtt_server.capath);
        printf("mosquitto_tls_set(mosq, %s, %s)\n", rio_cfg.mqtt_server.certfile, rio_cfg.mqtt_server.keyfile);
#endif
        if (ret != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_tls_set() error: %d\n", ret);
            exit(0);
        }

        ret = mosquitto_tls_insecure_set(*mosq, false);
        if (ret != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_tls_insecure_set() error: %d\n", ret);
            exit(0);
        }

        ret = mosquitto_tls_opts_set(*mosq, 1, rio_cfg.mqtt_server.tls_version, NULL);
        if (ret != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "mosquitto_tls_opts_set() error: %d\n", ret);
            exit(0);
        }
#ifdef DEBUG
        printf("SSL/TLS set done!\n");
#endif
    }
    mosquitto_message_callback_set(*mosq, message_callback);
    mosquitto_connect_callback_set(*mosq, connect_callback);
    mosquitto_subscribe_callback_set(*mosq, subscribe_callback);
#ifdef DEBUG
    mosquitto_publish_callback_set(*mosq, publish_callback);
#endif
    ret = mosquitto_connect(*mosq, rio_cfg.mqtt_server.ip, rio_cfg.mqtt_server.port, 600);
    if (ret != MOSQ_ERR_SUCCESS)
    {
        printf("connect error %d, errno: %d\n", ret, errno);
        exit(0);
    }
#ifdef DEBUG
    printf("mosquitto_connect(mosq, %s, %d, 600)\n", rio_cfg.mqtt_server.ip, rio_cfg.mqtt_server.port);
#endif

    return 0;
}

void mqtt_start(struct mosquitto **mosq)
{
#ifdef DEBUG
    printf("mosquitto_loop_start(mosq)\n");
#endif
    int ret = mosquitto_loop_start(*mosq);
    if (ret != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto_loop_start() error %d\n", ret);
        exit(0);
    }
}
