//
// Created by SHL on 28.12.2019.
//

#ifndef POWEROID_MODULES_CONTEXT_H
#define POWEROID_MODULES_CONTEXT_H

#define DEFAULT_DEVICE_ID "PWR-default"

#define CTX_LEN_DEVICE_ID 16

#define CTX_LEN_TOPIC 64

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
    bool connected;
    long retry_timeout;
};

struct WiFiContext {
    char ssid[CTX_LEN_WIFI_SSID];
    char pass[CTX_LEN_WIFI_PASS];
    bool connected;
    bool firstConnected;
    long retry_timeout;
};

struct UartContext {
    long speed = 115200;
    long retry_timeout;
};

struct Context {

    char device_id[CTX_LEN_DEVICE_ID];
    char pub_topic[CTX_LEN_TOPIC];
    char sub_topic[CTX_LEN_TOPIC];
    char sub_topic_raw[CTX_LEN_TOPIC];
    char sub_path[CTX_LEN_TOPIC];

    MqttConext mqtt;
    WiFiContext wifi;
    UartContext uart;

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
