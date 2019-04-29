#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "remoteIO_config.h"

struct config_t rio_cfg;
char *configfile = "./config.json";
void parse_mqtt_server(cJSON *root)
{
	cJSON *mqtt_server = cJSON_GetObjectItem(root, "mqtt_server");
	cJSON *ip = cJSON_GetObjectItem(mqtt_server, "ip");
	rio_cfg.mqtt_server.ip = (char *)strdup(ip->valuestring);
#ifdef DEBUG
	printf("ip: %s\n", rio_cfg.mqtt_server.ip);
#endif
	cJSON *port = cJSON_GetObjectItem(mqtt_server, "port");
	rio_cfg.mqtt_server.port = port->valueint;
#ifdef DEBUG
	printf("port: %d\n", rio_cfg.mqtt_server.port);
#endif
	cJSON *username = cJSON_GetObjectItem(mqtt_server, "username");
	rio_cfg.mqtt_server.username = (char *)strdup(username->valuestring);
#ifdef DEBUG
	printf("username: %s\n", rio_cfg.mqtt_server.username);
#endif
	cJSON *password = cJSON_GetObjectItem(mqtt_server, "password");
	rio_cfg.mqtt_server.password = (char *)strdup(password->valuestring);
#ifdef DEBUG
	printf("password: %s\n", rio_cfg.mqtt_server.password);
#endif
	cJSON *client_id = cJSON_GetObjectItem(mqtt_server, "client_id");
	rio_cfg.mqtt_server.client_id = (char *)strdup(client_id->valuestring);
#ifdef DEBUG
	printf("client_id: %s\n", rio_cfg.mqtt_server.client_id);
#endif
	cJSON *clean_session = cJSON_GetObjectItem(mqtt_server, "clean_session");
	rio_cfg.mqtt_server.clean_session = clean_session->valueint;
#ifdef DEBUG
	printf("clean_session: %d\n", rio_cfg.mqtt_server.clean_session);
#endif
	cJSON *tls_enable = cJSON_GetObjectItem(mqtt_server, "ssl/tls_enable");
	rio_cfg.mqtt_server.tls_enable = tls_enable->valueint;
#ifdef DEBUG
	printf("tls_enable: %d\n", rio_cfg.mqtt_server.tls_enable);
#endif
	cJSON *cafile = cJSON_GetObjectItem(mqtt_server, "cafile");
	rio_cfg.mqtt_server.cafile = strdup(cafile->valuestring);
#ifdef DEBUG
	printf("rio_cfg.mqtt_server.cafile: %s\n", rio_cfg.mqtt_server.cafile);
#endif

	cJSON *capath = cJSON_GetObjectItem(mqtt_server, "capath");
	rio_cfg.mqtt_server.capath = strdup(capath->valuestring);
#ifdef DEBUG
	printf("rio_cfg.mqtt_server.capath: %s\n", rio_cfg.mqtt_server.capath);
#endif

	cJSON *certfile = cJSON_GetObjectItem(mqtt_server, "certfile");
	if (strlen(certfile->valuestring) == 0)
		rio_cfg.mqtt_server.certfile = NULL;
	else
		rio_cfg.mqtt_server.certfile = strdup(certfile->valuestring);
#ifdef DEBUG
	if (rio_cfg.mqtt_server.certfile)
		printf("rio_cfg.mqtt_server.certfile: %s\n", rio_cfg.mqtt_server.certfile);
#endif
	cJSON *keyfile = cJSON_GetObjectItem(mqtt_server, "keyfile");
	if (strlen(keyfile->valuestring) == 0)
		rio_cfg.mqtt_server.keyfile = NULL;
	else
		rio_cfg.mqtt_server.keyfile = strdup(keyfile->valuestring);
#ifdef DEBUG
	if (rio_cfg.mqtt_server.keyfile)
		printf("rio_cfg.mqtt_server.keyfile: %s\n", rio_cfg.mqtt_server.keyfile);
#endif
	cJSON *tls_version = cJSON_GetObjectItem(mqtt_server, "tls_version");
	if (strcmp(tls_version->valuestring, "1") == 0 || strcmp(tls_version->valuestring, "1.0") == 0)
		rio_cfg.mqtt_server.tls_version = strdup("tlsv1");
	else if (strcmp(tls_version->valuestring, "1.1") == 0)
		rio_cfg.mqtt_server.tls_version = strdup("tlsv1.1");
	else
		rio_cfg.mqtt_server.tls_version = strdup("tlsv1.2");
#ifdef DEBUG
	printf("tls_version: %s\n", rio_cfg.mqtt_server.tls_version);
#endif
}

void parse_modbus_info(cJSON *root)
{
	cJSON *modbus_info = cJSON_GetObjectItem(root, "modbus_info");
	cJSON *ip = cJSON_GetObjectItem(modbus_info, "ip");
	strcpy(rio_cfg.modbus_info.ip, ip->valuestring);
#ifdef DEBUG
	printf("modbus_info->ip: %s\n", rio_cfg.modbus_info.ip);
#endif
	cJSON *port = cJSON_GetObjectItem(modbus_info, "port");
	rio_cfg.modbus_info.port = port->valueint;
#ifdef DEBUG
	printf("modbus_info->port: %d\n", rio_cfg.modbus_info.port);
#endif
	cJSON *slave_addr = cJSON_GetObjectItem(modbus_info, "slave");
	rio_cfg.modbus_info.slave_addr = slave_addr->valueint;
#ifdef DEBUG
	printf("modbus_info->slave_addr: %d\n", rio_cfg.modbus_info.slave_addr);
#endif
	cJSON *delay_ms = cJSON_GetObjectItem(modbus_info, "period_ms");
	rio_cfg.modbus_info.delay_ms = delay_ms->valueint;
	if (rio_cfg.modbus_info.delay_ms < 100)
		rio_cfg.modbus_info.delay_ms = 100;
#ifdef DEBUG
	printf("modbus_info->delay_ms: %d\n", rio_cfg.modbus_info.delay_ms);
#endif
}

void parse_resources_info(cJSON *root)
{
	cJSON *resources = cJSON_GetObjectItem(root, "resources");
	rio_cfg.n_resources = cJSON_GetArraySize(resources);
#ifdef DEBUG
	printf("resources size:%d\n", rio_cfg.n_resources);
#endif

	rio_cfg.resources = (struct resource_t *)malloc(sizeof(struct resource_t) * rio_cfg.n_resources);
	if (rio_cfg.resources == NULL)
	{
		printf("resurces malloc error\n");
		exit(0);
	}

	for (int i = 0; i < rio_cfg.n_resources; ++i)
	{
		cJSON *resource = cJSON_GetArrayItem(resources, i);
		cJSON *name = cJSON_GetObjectItem(resource, "name");
		strcpy(rio_cfg.resources[i].name, name->valuestring);
#ifdef DEBUG
		printf("resources[%d].name: %s\n", i, rio_cfg.resources[i].name);
#endif
		cJSON *get = cJSON_GetObjectItem(resource, "operation_get");
		if (get)
		{
			rio_cfg.resources[i].get = (struct operation_get_t *)malloc(sizeof(struct operation_get_t));
			if (rio_cfg.resources[i].get == NULL)
			{
				printf("operation_get malloc error\n");
				exit(0);
			}

			cJSON *func_code = cJSON_GetObjectItem(get, "func_code");
			rio_cfg.resources[i].get->func_code = func_code->valueint;
#ifdef DEBUG
			printf("resources[%d].get->func_fode: %d\n", i, rio_cfg.resources[i].get->func_code);
#endif
			cJSON *start_addr = cJSON_GetObjectItem(get, "start_addr");
			rio_cfg.resources[i].get->start_addr = start_addr->valueint;
#ifdef DEBUG
			printf("resources[%d].get->start_addr: %d\n", i, rio_cfg.resources[i].get->start_addr);
#endif
			cJSON *count = cJSON_GetObjectItem(get, "count");
			rio_cfg.resources[i].get->count = count->valueint;
#ifdef DEBUG
			printf("resources[%d].get->count: %d\n", i, rio_cfg.resources[i].get->count);
#endif
			cJSON *topic = cJSON_GetObjectItem(get, "topic");
			strcpy(rio_cfg.resources[i].get->topic, topic->valuestring);
#ifdef DEBUG
			printf("resources[%d],get->topic: %s\n", i, rio_cfg.resources[i].get->topic);
#endif
			cJSON *qos = cJSON_GetObjectItem(get, "qos");
			rio_cfg.resources[i].get->qos = qos->valueint;
#ifdef DEBUG
			printf("resources[%d],get->qos: %d\n", i, rio_cfg.resources[i].get->qos);
#endif
		}
		cJSON *set = cJSON_GetObjectItem(resource, "operation_set");
		if (set)
		{
			rio_cfg.resources[i].set = (struct operation_set_t *)malloc(sizeof(struct operation_set_t));
			if (rio_cfg.resources[i].set == NULL)
			{
				printf("operation_set malloc error\n");
				exit(0);
			}
			cJSON *func_code = cJSON_GetObjectItem(set, "func_code");
			rio_cfg.resources[i].set->func_code = func_code->valueint;
#ifdef DEBUG
			printf("resources[%d].set->func_fode: %d\n", i, rio_cfg.resources[i].set->func_code);
#endif
			cJSON *start_addr = cJSON_GetObjectItem(set, "start_addr");
			rio_cfg.resources[i].set->start_addr = start_addr->valueint;
#ifdef DEBUG
			printf("resources[%d].set->start_addr: %d\n", i, rio_cfg.resources[i].set->start_addr);
#endif
			cJSON *topic = cJSON_GetObjectItem(set, "topic");
			strcpy(rio_cfg.resources[i].set->topic, topic->valuestring);
#ifdef DEBUG
			printf("resources[%d].set->topic: %s\n", i, rio_cfg.resources[i].set->topic);
#endif
			cJSON *qos = cJSON_GetObjectItem(set, "qos");
			rio_cfg.resources[i].set->qos = qos->valueint;
#ifdef DEBUG
			printf("resources[%d].set->qos: %d\n", i, rio_cfg.resources[i].set->qos);
#endif
		}
	}
}

void config_parse()
{
	FILE *f;
	long len;
	char *data;
	f = fopen(configfile, "rb");
	if (!f)
	{
		printf("open file error\n");
		exit(0);
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = (char *)malloc(len + 1);
	fread(data, 1, len, f);
	fclose(f);

	cJSON *root = cJSON_Parse(data);
	if (!root)
	{
		printf("cJSON_Parse error: %s\n", cJSON_GetErrorPtr());
		exit(0);
	}

	parse_mqtt_server(root);
	parse_modbus_info(root);
	parse_resources_info(root);

	cJSON_Delete(root);
	free(data);
}
