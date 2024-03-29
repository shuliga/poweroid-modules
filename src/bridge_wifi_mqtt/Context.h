//
// Created by SHL on 28.12.2019.
//

#ifndef POWEROID_MODULES_CONTEXT_H
#define POWEROID_MODULES_CONTEXT_H

#include "stdint.h"

#define DEFAULT_PWR_DEVICE_ID "PWR-null"

#define CTX_LEN_DEVICE_ID 28

#define CTX_LEN_MQTT_PASS 16
#define CTX_LEN_MQTT_USER 32
#define CTX_LEN_MQTT_HOST 32
#define CTX_LEN_MQTT_PORT 6
#define CTX_LEN_MQTT_SERVICE 16
#define CTX_LEN_MQTT_CUSTOMER 16
#define CTX_LEN_MQTT_ADDRESS 32

#define CTX_LEN_WIFI_PASS 16
#define CTX_LEN_WIFI_SSID 16

#define CTX_LEN_UART_TOKEN 1


struct MqttContext {
    char host[CTX_LEN_MQTT_HOST + 1];
    char port[CTX_LEN_MQTT_PORT + 1];
    char user[CTX_LEN_MQTT_USER + 1];
    char pass[CTX_LEN_MQTT_PASS + 1];
    char address[CTX_LEN_MQTT_ADDRESS + 1];
    char customer[CTX_LEN_MQTT_CUSTOMER + 1];
    char service[CTX_LEN_MQTT_SERVICE + 1];
    long retry_timeout;
};

struct WiFiContext {
    char ssid[CTX_LEN_WIFI_SSID + 1];
    char pass[CTX_LEN_WIFI_PASS + 1];
    long retry_timeout;
};

struct UartContext {
    char device_id[CTX_LEN_DEVICE_ID + 1];
    long speed;
    uint8_t token;
    long retry_timeout;
};

struct Context {

    MqttContext mqtt;
    WiFiContext wifi;
    UartContext uart;

};

void loadDefaultContext(Context &ctx);
void storeContext(Context &ctx);
void loadContext(Context &ctx);
void resetEepromContext(Context &ctx);
void buildSubTopic(Context &ctx, const char * module_id);
void buildPubTopic(Context &ctx, const char * module_id);
unsigned long hash(Context &ctx);
void resetUartDeviceId(Context &ctx);

#endif //POWEROID_MODULES_CONTEXT_H
