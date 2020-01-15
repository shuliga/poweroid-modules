//
// Created by SHL on 28.12.2019.
//
#include <Arduino.h>
#include <EEPROM.h>
#include "Context.h"
#include "ParserConstants.h"

const char *address = "ua-lv-79004-nv-7-4";

const char *ssid = "SHL-Net-2";
const char *pass = "76543210";

const char *mqtt_server = "farmer.cloudmqtt.com";
const char *mqtt_port = "12674";
const char *mqtt_user = "thermo-01";
const char *mqtt_pass = "ThermoPass";

const char *service = "rent";

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

    strcpy(ctx.device_id, DEFAULT_DEVICE_ID);
    ctx.uart.speed = uart_baud;
}

void loadContext(Context &ctx) {
    EEPROM.begin(sizeof(ctx));
    EEPROM.get(0, ctx);
    EEPROM.end();
    if (ctx.checksum != hash(ctx)) {
        loadDefaultContext(ctx);
        Serial.println("Checksum error. Default context loaded.");
    }
}

void resetEepromContext(Context &ctx) {
    EEPROM.begin(sizeof(ctx));
    for (unsigned long a = 0; a < sizeof(ctx); a++) {
        EEPROM.write(a, 0);
    }
    EEPROM.end();
}

void storeContext(Context &ctx) {
    EEPROM.begin(sizeof(ctx));
    ctx.checksum = hash(ctx);
    EEPROM.put(0, ctx);
    EEPROM.commit();
}

void buildSubTopic(Context &ctx) {
    sprintf(ctx.sub_path, "%s/%s/%s", ctx.mqtt.service, ctx.mqtt.address, ctx.device_id);
    sprintf(ctx.sub_topic, "%s/%s/#", ctx.sub_path, MSG_TYPE_CMD);
    sprintf(ctx.sub_topic_raw, "%s/%s/#", ctx.sub_path, MSG_TYPE_IN);
}

void buildPubTopic(Context &ctx) {
    sprintf(ctx.pub_topic, "%s/%s/%s", ctx.mqtt.service, ctx.mqtt.address, ctx.device_id);
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

