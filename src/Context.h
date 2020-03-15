//
// Created by SHL on 28.12.2019.
//

#ifndef POWEROID_MODULES_CONTEXT_H
#define POWEROID_MODULES_CONTEXT_H

#define DEFAULT_PWR_DEVICE_ID "PWR-null"

#define CTX_LEN_DEVICE_ID 24

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
    char host[CTX_LEN_MQTT_HOST + 1];
    char port[CTX_LEN_MQTT_PORT + 1];
    char user[CTX_LEN_MQTT_USER + 1];
    char pass[CTX_LEN_MQTT_PASS + 1];
    char address[CTX_LEN_MQTT_ADDRESS + 1];
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
    long retry_timeout;
};

struct Context {

    char pub_topic[CTX_LEN_TOPIC + 1];
    char sub_topic[CTX_LEN_TOPIC + 1];
    char sub_topic_raw[CTX_LEN_TOPIC + 1];
    char sub_path[CTX_LEN_TOPIC + 1];

    MqttConext mqtt;
    WiFiContext wifi;
    UartContext uart;

};

void loadDefaultContext(Context &ctx);
void storeContext(Context &ctx);
void loadContext(Context &ctx);
void resetEepromContext(Context &ctx);
void buildSubTopic(Context &ctx);
void buildPubTopic(Context &ctx);
void buildPubTopic(Context &ctx, const char * device_id);
unsigned long hash(Context &ctx);

#endif //POWEROID_MODULES_CONTEXT_H
