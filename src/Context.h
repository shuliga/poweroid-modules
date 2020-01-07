//
// Created by SHL on 28.12.2019.
//

#ifndef POWEROID_MODULES_CONTEXT_H
#define POWEROID_MODULES_CONTEXT_H

#define CTX_LEN_MQTT_PASS 16
#define CTX_LEN_MQTT_USER 32
#define CTX_LEN_MQTT_HOST 32
#define CTX_LEN_MQTT_PORT 6
#define CTX_LEN_MQTT_SERVICE 16
#define CTX_LEN_MQTT_ADDRESS 32

#define CTX_LEN_WIFI_PASS 16
#define CTX_LEN_WIFI_SSID 16


struct MqttConext {
    char host[CTX_LEN_MQTT_HOST];
    char port[CTX_LEN_MQTT_PORT];
    char user[CTX_LEN_MQTT_USER];
    char pass[CTX_LEN_MQTT_PASS];
    char address[CTX_LEN_MQTT_ADDRESS];
    char service[CTX_LEN_MQTT_SERVICE];
    long retry_timeout;
};

struct WiFiContext {
    char ssid[CTX_LEN_WIFI_SSID];
    char pass[CTX_LEN_WIFI_PASS];
    long retry_timeout;
};

struct UartContext {
    int speed = 115200;
    long retry_timeout;
};

struct Context {
    char device_id[16];
    char pub_topic[64];
    char sub_topic[64];
    char sub_topic_raw[64];
    char sub_path[64];
    MqttConext mqtt;
    WiFiContext wifi;
    UartContext uart;
    bool verbose = true;
    bool mute = true;
    unsigned long checksum;
};

void loadDefaultContext(Context &ctx);
void storeContext(Context &ctx);
void loadContext(Context &ctx);
void resetEepromContext(Context &ctx);
void buildSubTopic(Context &ctx);
void buildPubTopic(Context &ctx);
unsigned long hash(Context &ctx);

#endif //POWEROID_MODULES_CONTEXT_H
