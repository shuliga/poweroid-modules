//
// Created by SHL on 28.12.2019.
//
#include <Arduino.h>
#include <EEPROM.h>
#include "Context.h"
#include "ParserConstants.h"

// Address pattern: <country code #2>-<City code #2>-<postal code>-<street code #2>-<building no.>-<apt. no.>
const char *address = "ua-test";

const char *ssid = "SHL-Net-2";
const char *pass = "76543210";

const char *mqtt_server = "farmer.cloudmqtt.com";
const char *mqtt_port = "12674";
const char *mqtt_user = "thermo-01";
const char *mqtt_pass = "ThermoPass";

const char *service = "rent";

size_t eepromSize(const Context &ctx);

const int uart_baud = 115200;

void loadDefaultContext(Context &ctx) {
    strcpy(ctx.mqtt.service, service);
    strcpy(ctx.mqtt.address, address);
    strcpy(ctx.mqtt.host, mqtt_server);
    strcpy(ctx.mqtt.user, mqtt_user);
    strcpy(ctx.mqtt.pass, mqtt_pass);
    strcpy(ctx.mqtt.port, mqtt_port);

    strcpy(ctx.wifi.ssid, ssid);
    strcpy(ctx.wifi.pass, pass);
}

void loadContext(Context &ctx) {
    EEPROM.begin(eepromSize(ctx));
    EEPROM.get(0, ctx);
    unsigned long checksum;
    EEPROM.get(sizeof(ctx), checksum);
    EEPROM.end();
    strcpy(ctx.uart.device_id, DEFAULT_PWR_DEVICE_ID);
    ctx.uart.speed = uart_baud;
    if (checksum != hash(ctx)) {
        loadDefaultContext(ctx);
        Serial.println("Checksum error. Default context loaded.");
    }
}

size_t eepromSize(const Context &ctx) { return sizeof(ctx) + sizeof(long); }

void resetEepromContext(Context &ctx) {
    EEPROM.begin(eepromSize(ctx));
    for (unsigned long a = 0; a < sizeof(ctx); a++) {
        EEPROM.write(a, 0);
    }
    EEPROM.end();
}

void storeContext(Context &ctx) {
    EEPROM.begin(eepromSize(ctx));
    unsigned long checksum = hash(ctx);
    EEPROM.put(0, ctx);
    EEPROM.put(sizeof(ctx), checksum);
    EEPROM.commit();
}

void buildSubTopic(Context &ctx, const char *device_id) {
    sprintf(ctx.sub_path, "%s/%s", ctx.mqtt.service, ctx.mqtt.address);
    sprintf(ctx.sub_topic, "%s/%s/%s/#", ctx.sub_path, ctx.uart.device_id, MSG_TYPE_CMD);
    sprintf(ctx.sub_topic_raw, "%s/%s/%s", ctx.sub_path, ctx.uart.device_id, MSG_TYPE_RAW_IN);
    sprintf(ctx.sub_topic_exec_at, "%s/%s/%s", ctx.sub_path, device_id, MSG_TYPE_EXEC_AT);
}

void buildPubTopic(Context &ctx) {
    sprintf(ctx.pub_topic, "%s/%s/%s", ctx.mqtt.service, ctx.mqtt.address, ctx.uart.device_id);
}

void buildPubTopic(Context &ctx, const char * device_id) {
    sprintf(ctx.pub_topic, "%s/%s/%s", ctx.mqtt.service, ctx.mqtt.address, device_id && strlen(device_id) > 0 ?  device_id : ctx.uart.device_id);
}

unsigned long hash(byte *data, unsigned long size) {
    unsigned long hash = 19;
    for (unsigned long i = 0; i < size; i++) {
        byte c = *data++;
        if (c != 0) { // usually when doing on strings this wouldn't be needed
            hash *= c;
        }
        hash += 7;
    }
    return hash;
}

unsigned long hash(Context &ctx) {
    return hash((byte *) &ctx, sizeof(Context));
}