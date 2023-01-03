//
// Created by SHL on 28.12.2019.
//
#include <Arduino.h>
#include <EEPROM.h>
#include "Global.h"
#include "Context.h"
#include "ParserConstants.h"

const char *service = "test";
const char *customer = "test-customer";

// Address pattern: <country code #2>-<City code #2>-<postal code>-<street code #2>-<building no.>-<apt. no.>
const char *address = "ua-test";

const char *ssid = "SHL-Net-2";
const char *pass = "76543210";

const char *mqtt_server = "mqtt.poweroid.io";
const char *mqtt_port = "1883";
const char *mqtt_user = "thermo-01";
const char *mqtt_pass = "ThermoPass";
const int uart_baud = 115200;

size_t eepromSize(const Context &ctx);


void loadDefaultContext(Context &ctx) {
    strcpy(ctx.mqtt.service, service);
    strcpy(ctx.mqtt.address, address);
    strcpy(ctx.mqtt.customer, customer);
    strcpy(ctx.mqtt.host, mqtt_server);
    strcpy(ctx.mqtt.user, mqtt_user);
    strcpy(ctx.mqtt.pass, mqtt_pass);
    strcpy(ctx.mqtt.port, mqtt_port);

    strcpy(ctx.wifi.ssid, ssid);
    strcpy(ctx.wifi.pass, pass);

    ctx.uart.speed = uart_baud;
    ctx.uart.token = 0;
}

void loadContext(Context &ctx) {
    EEPROM.begin(eepromSize(ctx));
    EEPROM.get(0, ctx);
    unsigned long checksum;
    EEPROM.get(sizeof(ctx), checksum);
    EEPROM.end();
    Serial.print("Checksum fetched: ");
    Serial.println(checksum);
    Serial.print("Hash calculated: ");
    Serial.println(hash(ctx));

    if (checksum != hash(ctx)) {
        loadDefaultContext(ctx);
        Serial.println("Checksum error. Default context loaded.");
    }
    resetUartDeviceId(ctx);
    GLOBAL.cmd.updateToken = ctx.uart.token > 0;
}

void resetUartDeviceId(Context &ctx) { strcpy(ctx.uart.device_id, DEFAULT_PWR_DEVICE_ID); }

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
    unsigned long checksum= hash(ctx);
    Serial.print("Config stored. Checksum: ");
    Serial.println(checksum);
    EEPROM.put(0, ctx);
    EEPROM.put(sizeof(ctx), checksum);
    EEPROM.commit();
    delay(500);
    EEPROM.end();
}

void buildSubTopic(Context &ctx, const char *module_id) {
    sprintf(GLOBAL.topics.prefix, "%s/%s/%s", ctx.mqtt.service, ctx.mqtt.customer, ctx.mqtt.address);
    sprintf(GLOBAL.topics.sub_uart_device_topic, "%s/%s/#", GLOBAL.topics.prefix, ctx.uart.device_id);
    sprintf(GLOBAL.topics.sub_module_topic, "%s/%s/#", GLOBAL.topics.prefix, module_id);
}

void buildPubTopic(Context &ctx, const char * module_id) {
    sprintf(GLOBAL.topics.pub_topic, "%s/%s", GLOBAL.topics.prefix, module_id && strlen(module_id) > 0 ? module_id : ctx.uart.device_id);
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