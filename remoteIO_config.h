#ifndef __REMOTEIO_CONFIG_H__
#define __REMOTEIO_CONFIG_H__
#include <stdio.h>
#include <stdbool.h>

#include "cJSON.h"

struct mqtt_server_t
{
    char *ip;
    int port;
    char *username;
    char *password;
    char *client_id;
    bool clean_session;
    bool tls_enable;
    char *cafile;
    char *capath;
    char *certfile;
    char *keyfile;
    char *tls_version;
};

#define MODBUS_INFO_IP_LEN 16

struct modbus_info_t
{
    char ip[MODBUS_INFO_IP_LEN];
    int port;
    int slave_addr;
    int delay_ms;
};

#define OPERATION_TOPIC_LEN 32
struct operation_get_t
{
    int func_code;
    int start_addr;
    int count;
    char topic[OPERATION_TOPIC_LEN];
    int qos;
};
struct operation_set_t
{
    int func_code;
    int start_addr;
    char topic[OPERATION_TOPIC_LEN];
    int qos;
};

#define RESOURCE_NAME_LEN 16
struct resource_t
{
    char name[RESOURCE_NAME_LEN];
    struct operation_get_t *get;
    struct operation_set_t *set;
};

struct config_t
{
    struct mqtt_server_t mqtt_server;
    struct modbus_info_t modbus_info;
    int n_resources;
    struct resource_t *resources;
};

extern struct config_t rio_cfg;
void config_parse();
#endif
