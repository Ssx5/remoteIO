#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include "remoteIO_modbus.h"
#include "remoteIO_config.h"
#include "remoteIO_mqtt.h"

modbus_t *modbus_ctx;

int modbus_init(modbus_t **ctx)
{
    *ctx = modbus_new_tcp(rio_cfg.modbus_info.ip, rio_cfg.modbus_info.port);
    if (*ctx == NULL)
    {
        fprintf(stderr, "modbus_new_tcp error\n");
        exit(0);
    }
#ifdef DEBUG
    printf("modbus_new_tcp(%s, %d)\n", rio_cfg.modbus_info.ip, rio_cfg.modbus_info.port);
#endif

    modbus_set_slave(*ctx, rio_cfg.modbus_info.slave_addr);
#ifdef DEBUG
    printf("modbus_set_slave(%d)\n", rio_cfg.modbus_info.slave_addr);
#endif

    if (modbus_connect(*ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(*ctx);
        exit(0);
    }

    fprintf(stdout, "modbus_connect(%s, %d) success!\n",rio_cfg.modbus_info.ip, rio_cfg.modbus_info.port);

#ifdef DEBUG
    //modbus_set_debug(*ctx, TRUE);
#endif

    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 100 * 1000;
    modbus_set_response_timeout(*ctx, &t);
#ifdef DEBUG
    printf("modbus_set_response_timeout(%ld,%ld)\n", t.tv_sec, t.tv_usec);
#endif
    return 0;
}

void *modbus_schedule(void *arg)
{
    uint8_t bitbuf[32];
    uint16_t regbuf[16];
    int ret = 0;
    while (1)
    {
        for (int i = 0; i < rio_cfg.n_resources; ++i)
        {
            struct operation_get_t *get = rio_cfg.resources[i].get;
            memset(bitbuf, 0, sizeof(bitbuf));
            memset(regbuf, 0, sizeof(regbuf));
            if (get)
            {
                switch (get->func_code)
                {
                case 1:
                    ret = modbus_read_bits(modbus_ctx, get->start_addr, get->count, bitbuf);
                    if (ret < 0)
                    {
                        fprintf(stderr, "%s: ", rio_cfg.resources[i].name);
                        fprintf(stderr, "Read Failed: %s\n", modbus_strerror(errno));
                        continue;
                    }
#ifdef DEBUG
                    printf("%s: ", rio_cfg.resources[i].name);
                    for (int j = 0; j < ret; ++j)
                    {
                        printf("%.2x ", bitbuf[i]);
                    }
                    printf("\n");
#endif
                    mosquitto_publish(mosq, NULL, get->topic, ret, bitbuf, get->qos, false);
#ifdef DEBUG
                    printf("mosquitto_publish(mosq, NULL, %s, %d, %s)\n", get->topic, ret, bitbuf);
#endif
                    break;
                case 2:
                    ret = modbus_read_input_bits(modbus_ctx, get->start_addr, get->count, bitbuf);
                    if (ret < 0)
                    {
                        fprintf(stderr, "%s: ", rio_cfg.resources[i].name);
                        fprintf(stderr, "Read Failed: %s\n", modbus_strerror(errno));
                        continue;
                    }
#ifdef DEBUG
                    printf("%s: ", rio_cfg.resources[i].name);
                    for (int j = 0; j < ret; ++j)
                    {
                        printf("%.2x ", bitbuf[i]);
                    }
                    printf("\n");
#endif
                    mosquitto_publish(mosq, NULL, get->topic, ret, bitbuf, get->qos, false);
#ifdef DEBUG
                    printf("mosquitto_publish(mosq, NULL, %s, %d, %s)\n", get->topic, ret, bitbuf);
#endif

                    break;
                case 3:
                    ret = modbus_read_registers(modbus_ctx, get->start_addr, get->count, regbuf);
                    if (ret < 0)
                    {
                        fprintf(stderr, "%s: ", rio_cfg.resources[i].name);
                        fprintf(stderr, "Read Failed: %s\n", modbus_strerror(errno));
                        continue;
                    }
#ifdef DEBUG
                    printf("%s: ", rio_cfg.resources[i].name);
                    for (int j = 0; j < ret; ++j)
                    {
                        printf("%.4x ", regbuf[i]);
                    }
                    printf("\n");
#endif
                    mosquitto_publish(mosq, NULL, get->topic, ret * 2, regbuf, get->qos, false);
#ifdef DEBUG
                    printf("mosquitto_publish(mosq, NULL, %s, %d, %s)\n", get->topic, ret, (char *)regbuf);
#endif
                    break;
                case 4:
                    ret = modbus_read_input_registers(modbus_ctx, get->start_addr, get->count, regbuf);
                    if (ret < 0)
                    {
                        fprintf(stderr, "%s: ", rio_cfg.resources[i].name);
                        fprintf(stderr, "Read Failed: %s\n", modbus_strerror(errno));
                        continue;
                    }
#ifdef DEBUG
                    printf("%s: ", rio_cfg.resources[i].name);
                    for (int j = 0; j < ret; ++j)
                    {
                        printf("%.4x ", regbuf[i]);
                    }
                    printf("\n");
#endif
                    mosquitto_publish(mosq, NULL, get->topic, ret * 2, regbuf, get->qos, false);
#ifdef DEBUG
                    printf("mosquitto_publish(mosq, NULL, %s, %d, %s)\n", get->topic, ret * 2, (char *)regbuf);
#endif
                    break;
                default:
                    break;
                }
            }
        }
        usleep(rio_cfg.modbus_info.delay_ms * 1000);
    }
}
