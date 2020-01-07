//
// Created by SHL on 04.01.2020.
//

#include "AtCommands.h"
#include <Arduino.h>

#define CMD_RESTART "RESTART"
#define CMD_PERSIST "PERSIST"

#define CMD_WIFI_INFO "WIFI_INFO"
#define CMD_WIFI_PASS "WIFI_PASS"
#define CMD_WIFI_SSID "WIFI_SSID"

#define CMD_MQTT_INFO "MQTT_INFO"
#define CMD_MQTT_USER "MQTT_USER"
#define CMD_MQTT_HOST "MQTT_HOST"
#define CMD_MQTT_PORT "MQTT_PORT"
#define CMD_MQTT_PASS "MQTT_PASS"
#define CMD_MQTT_ADDRESS "MQTT_ADDRESS"
#define CMD_MQTT_SERVICE "MQTT_SERVICE"
#define CMD_MQTT_CID "MQTT_CID"

const char *AtCommands::process(const char *atCommand) {
    if (strcmp("AT", atCommand) == 0){
        return "OK";
    }
    if (strncmp("AT+", atCommand, 3) == 0) {
        char cmd[64];
        strcpy(cmd, atCommand + 3);

        if(startsWith(cmd, CMD_RESTART)){
            restart();
        }

        if(startsWith(cmd, CMD_PERSIST)){
            persist();
            return "Config stored.";
        }

        if(startsWith(cmd, CMD_WIFI_SSID)){
            return setOrGetCtxValue(ctx->wifi.ssid, getValue(cmd), CTX_LEN_WIFI_SSID);
        }
        if(startsWith(cmd, CMD_WIFI_PASS)){
            return setOrGetCtxValue(ctx->wifi.pass, getValue(cmd), CTX_LEN_WIFI_PASS);
        }
        if(startsWith(cmd, CMD_WIFI_INFO)){
            uint8_t size  =
                    strlen(ctx->wifi.pass) +
                    strlen(ctx->wifi.ssid) +
                    strlen(CMD_WIFI_SSID) +
                    strlen(CMD_WIFI_PASS) + 7;
            char *info = new char[size];
            sprintf(info, "%s: %s\n%s: %s",
                    CMD_WIFI_SSID, ctx->wifi.ssid,
                    CMD_WIFI_PASS, ctx->wifi.pass);
            return info;
        }

        if(startsWith(cmd, CMD_MQTT_INFO)){
            uint8_t size  =
                    strlen(ctx->mqtt.host) +
                    strlen(ctx->mqtt.port) +
                    strlen(ctx->mqtt.user) +
                    strlen(ctx->mqtt.pass) +
                    strlen(ctx->mqtt.service) +
                    strlen(ctx->mqtt.address) +
                    strlen(CMD_MQTT_HOST) +
                    strlen(CMD_MQTT_PORT) +
                    strlen(CMD_MQTT_USER) +
                    strlen(CMD_MQTT_PASS) +
                    strlen(CMD_MQTT_SERVICE) +
                    strlen(CMD_MQTT_ADDRESS) +
                    18;
            char *info = new char[size];
            sprintf(info, "%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s",
                    CMD_MQTT_HOST, ctx->mqtt.host,
                    CMD_MQTT_PORT, ctx->mqtt.port,
                    CMD_MQTT_USER, ctx->mqtt.user,
                    CMD_MQTT_PASS, ctx->mqtt.pass,
                    CMD_MQTT_SERVICE, ctx->mqtt.service,
                    CMD_MQTT_ADDRESS, ctx->mqtt.address
                    );
            return info;
        }
        if(startsWith(cmd, CMD_MQTT_HOST)){
            return setOrGetCtxValue(ctx->mqtt.host, getValue(cmd), CTX_LEN_MQTT_HOST);
        }
        if(startsWith(cmd, CMD_MQTT_PORT)){
            return setOrGetCtxValue(ctx->mqtt.port, getValue(cmd), CTX_LEN_MQTT_PORT);
        }
        if(startsWith(cmd, CMD_MQTT_USER)){
            return setOrGetCtxValue(ctx->mqtt.user, getValue(cmd), CTX_LEN_MQTT_USER);
        }
        if(startsWith(cmd, CMD_MQTT_PASS)){
            return setOrGetCtxValue(ctx->mqtt.pass, getValue(cmd), CTX_LEN_MQTT_PASS);
        }
        if(startsWith(cmd, CMD_MQTT_SERVICE)){
            return setOrGetCtxValue(ctx->mqtt.service, getValue(cmd), CTX_LEN_MQTT_SERVICE);
        }
        if(startsWith(cmd, CMD_MQTT_ADDRESS)){
            return setOrGetCtxValue(ctx->mqtt.address, getValue(cmd), CTX_LEN_MQTT_ADDRESS);
        }

    }
    return NULL;
}

void AtCommands::restart() {
    ESP.restart();
}


bool AtCommands::startsWith(const char * _cmd, const char * pref){
    return strncmp(pref, _cmd, strlen(pref)) == 0;
}

const char * AtCommands::getValue(const char * _cmd){
    char * v = strchr(_cmd, '=');
    return v == NULL ? NULL : v + 1;
}

AtCommands::AtCommands(Context *ctx) : ctx(ctx) {}

const char * AtCommands::setOrGetCtxValue(char *ctx_val, const char *value, uint8_t length) {
    if (value != NULL){
        strncpy(ctx_val, value, length);
    }
    return ctx_val;
}

bool  AtCommands::strEndsWith(const char * str, const char * suffix) {

    if( str == NULL || suffix == NULL )
        return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len)
        return 0;

    return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

void AtCommands::persist() {
    storeContext(*ctx);
}
