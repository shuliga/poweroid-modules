//
// Created by SHL on 04.01.2020.
//

#include "AtCommands.h"
#include <Arduino.h>
#include "Global.h"
#include "Common.h"

static const char* CMD_RESTART="RESTART";
static const char* CMD_PERSIST="PERSIST";
static const char* CMD_CONNECT="CONNECT";
static const char* CMD_VERBOSE="VERBOSE";
static const char* CMD_MASTER="MASTER";

static const char* CMD_UART_INFO="UART_INFO";
static const char* UART_DEVICE_ID="UART_DEVICE_ID";
static const char* UART_BAUD="UART_BAUD";

static const char* CMD_WIFI_INFO="WIFI_INFO";
static const char* CMD_WIFI_PASS="WIFI_PASS";
static const char* CMD_WIFI_SSID="WIFI_SSID";
static const char* CMD_WIFI_TRY_CONNECT="WIFI_TRY_CONNECT";
static const char* CMD_WIFI_CONNECTED="WIFI_CONNECTED";

static const char* CMD_MQTT_INFO="MQTT_INFO";
static const char* CMD_MQTT_USER="MQTT_USER";
static const char* CMD_MQTT_HOST="MQTT_HOST";
static const char* CMD_MQTT_PORT="MQTT_PORT";
static const char* CMD_MQTT_PASS="MQTT_PASS";
static const char* CMD_MQTT_ADDRESS="MQTT_ADDRESS";
static const char* CMD_MQTT_SERVICE="MQTT_SERVICE";
static const char* CMD_MQTT_CUSTOMER="MQTT_CUSTOMER";
static const char* CMD_MQTT_CONNECTED="MQTT_CONNECTED";
static const char* CMD_MQTT_CONNECT="MQTT_CONNECT";

static const char* bool_values[2] = {"0", "1"};
static bool bool_val;

static const char* C_OK = "OK";
static const char* C_GET = ":";
static const char* C_SET = "=";
static const char* C_EMPTY = "";
static const char* C_CONFIG_STORED = "Config stored";

static const char* C_RES[3] = {NULL, NULL, NULL};

const char **AtCommands::process(const char *atCommand) {
    if (strcmp("AT", atCommand) == 0){
        return getInfo(const_cast<char*>(C_OK));
    }
    if (strncmp("AT+", atCommand, 3) == 0) {
        char cmd[64];
        strcpy(cmd, atCommand + 3);

        if(startsWith(cmd, CMD_RESTART)){
            restart();
        }

        if(startsWith(cmd, CMD_PERSIST)){
            persist();
            return getInfo(const_cast<char*>(C_CONFIG_STORED));
        }

        if(startsWith(cmd, CMD_MASTER)){
            return cmdSet(CMD_MASTER, processBool(cmd, GLOBAL.flag.master));
        }

        if(startsWith(cmd, CMD_CONNECT)){
            return cmdSet(CMD_CONNECT, processBool(cmd, GLOBAL.flag.connect));
        }

        if(startsWith(cmd, CMD_WIFI_TRY_CONNECT)){
            return cmdSet(CMD_WIFI_TRY_CONNECT, processBool(cmd, GLOBAL.flag.wifiTryConnect));
        }

        if(startsWith(cmd, CMD_VERBOSE)){
            return cmdSet(CMD_VERBOSE, processBool(cmd, GLOBAL.flag.verbose));
        }

        if(startsWith(cmd, CMD_UART_INFO)){
            const uint8_t size  =
                    16 +
                    7 +
                    2*3+1;
            static char info[size];
            sprintf(info, "%s: %s\n%s: %lu",
                    CMD_WIFI_SSID, ctx->uart.device_id,
                    CMD_WIFI_PASS, ctx->uart.speed
            );
            return getInfo(info);
        }

        if(startsWith(cmd, CMD_WIFI_SSID)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_WIFI_SSID, val, setOrGetCtxValue(ctx->wifi.ssid, val, CTX_LEN_WIFI_SSID));
        }

        if(startsWith(cmd, CMD_WIFI_PASS)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_WIFI_PASS, val, setOrGetCtxValue(ctx->wifi.pass, val, CTX_LEN_WIFI_PASS));
        }

        if(startsWith(cmd, CMD_WIFI_INFO)){
            const uint8_t size  =
                    16 +
                    16 +
                    1 +
                    16 +
                    16 +
                    16 +
                    3*3+1;
            static char info[size];
            sprintf(info, "%s: %s\n%s: %s\n%s: %s",
                    CMD_WIFI_SSID, ctx->wifi.ssid,
                    CMD_WIFI_PASS, ctx->wifi.pass,
                    CMD_WIFI_CONNECTED, bool_values[GLOBAL.status.wifiConnected]
                    );
            return getInfo(info);
        }

        if(startsWith(cmd, CMD_MQTT_INFO)){
            const uint8_t size  =
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    1 +
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    16 +
                    3*7 + 1;
            static char info[size];
            sprintf(info, "%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s",
                    CMD_MQTT_HOST, ctx->mqtt.host,
                    CMD_MQTT_PORT, ctx->mqtt.port,
                    CMD_MQTT_USER, ctx->mqtt.user,
                    CMD_MQTT_PASS, ctx->mqtt.pass,
                    CMD_MQTT_SERVICE, ctx->mqtt.service,
                    CMD_MQTT_CUSTOMER, ctx->mqtt.customer,
                    CMD_MQTT_ADDRESS, ctx->mqtt.address,
                    CMD_MQTT_CONNECTED, bool_values[GLOBAL.status.mqttConnected]
            );
            return getInfo(info);
        }

        if(startsWith(cmd, CMD_MQTT_CONNECT)){
            return cmdSet(CMD_MQTT_CONNECT, processBool(cmd, GLOBAL.flag.mqttConnect));
        }

        if(startsWith(cmd, CMD_MQTT_HOST)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_HOST, val, setOrGetCtxValue(ctx->mqtt.host, val, CTX_LEN_MQTT_HOST));
        }
        if(startsWith(cmd, CMD_MQTT_PORT)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_PORT, val, setOrGetCtxValue(ctx->mqtt.port, val, CTX_LEN_MQTT_PORT));
        }
        if(startsWith(cmd, CMD_MQTT_USER)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_USER, val, setOrGetCtxValue(ctx->mqtt.user, val, CTX_LEN_MQTT_USER));
        }
        if(startsWith(cmd, CMD_MQTT_PASS)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_PASS, val, setOrGetCtxValue(ctx->mqtt.pass, val, CTX_LEN_MQTT_PASS));
        }
        if(startsWith(cmd, CMD_MQTT_SERVICE)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_SERVICE, val, setOrGetCtxValue(ctx->mqtt.service, val, CTX_LEN_MQTT_SERVICE));
        }
        if(startsWith(cmd, CMD_MQTT_CUSTOMER)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_CUSTOMER, val, setOrGetCtxValue(ctx->mqtt.service, val, CTX_LEN_MQTT_CUSTOMER));
        }
        if(startsWith(cmd, CMD_MQTT_ADDRESS)){
            const char * val = getValue(cmd);
            return cmdSetOrGet(CMD_MQTT_ADDRESS, val, setOrGetCtxValue(ctx->mqtt.address, val, CTX_LEN_MQTT_ADDRESS));
        }

    }
    return NULL;
}

const char* AtCommands::processBool(const char * cmd, bool & target){
    const char * val = getValue(cmd);
    bool_val = val == NULL ? false : atoi(val);
    return bool_values[setOrGetValue(&target, val == NULL ? NULL : &bool_val)];
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

bool AtCommands::setOrGetValue(bool *_val, const bool * new_val) {
    if (new_val != NULL){
        *_val = *new_val;
    }
    return *_val;
}

void AtCommands::persist() {
    storeContext(*ctx);
}

void AtCommands::restart() {
    ESP.restart();
}

const char ** AtCommands::cmdGet(const char* cmd, const char* result){
    C_RES[0] = cmd;
    C_RES[1] = C_GET;
    C_RES[2] = result;
    return C_RES;
}

const char ** AtCommands::cmdSet(const char* cmd, const char* result){
    C_RES[0] = cmd;
    C_RES[1] = C_SET;
    C_RES[2] = result;
    return C_RES;
}

const char ** AtCommands::cmdSetOrGet(const char* cmd, const char* val, const char* result){
    C_RES[0] = cmd;
    C_RES[1] = val == NULL ? C_GET : C_SET;
    C_RES[2] = result;
    return C_RES;
}

const char ** AtCommands::getInfo(char* result){
    C_RES[0] = C_EMPTY;
    C_RES[1] = C_EMPTY;
    C_RES[2] = result;
    return C_RES;
}