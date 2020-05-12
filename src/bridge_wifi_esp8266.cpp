//
// Created by SHL on 23.12.2019.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Global.h"
#include "Context.h"
#include "CircularBuffer.h"
#include "AtCommands.h"
#include "PoweroidProcessor.h"
#include "MqttProcessor.h"
#include "ParserConstants.h"
#include "Common.h"
#include "BluetoothHC05.h"
#include "TimeClient.h"

#define LED 2

const char *DEVICE_ID = "PWM-BMU-01"; // Poweroid module, Bridge MQTT-UART
char DEVICE_ID_MAC[34];
char DEVICE_ID_ADDR[64];

char *DEVICE_MQTT_ID = DEVICE_ID_MAC;

unsigned long timestamp = 0;

Context CTX;

CircularBuffer IN, OUT;

WiFiClient wclient;
PubSubClient mqttClient(wclient);

AtCommands AT(&CTX);
PoweroidProcessor pwrProcessor(&IN, &OUT, &CTX);
MqttProcessor mqttProcessor(&IN, &OUT, &CTX, &mqttClient);

void printMQTTInfo();

void testConnect();

void loadContext();

void syncTimeAndStates();

void initBt();

void cleanSerial();

void processTimer();

void mqttTryConnect(PubSubClient &client);

void mqttSubscribe(PubSubClient &client);

void scanAvailableWiFiNetworks();

void mqtt_callback(char *topic, unsigned char *payload, unsigned int length) {
    mqttProcessor.process(topic, payload, length);
}

void loadContext() {
    Serial.println("Loading context");
    loadContext(CTX);
    buildSubTopic(CTX, DEVICE_ID);
}

void printMQTTInfo() {
    Serial.printf("MQTT UART path: %s#\r\n", GLOBAL.topics.sub_uart_topic);
    Serial.printf("MQTT device's RAW: %s\r\n", GLOBAL.topics.sub_device_topic);
}

void sendInitMessage(const char *device_id) {
    ParserModel initMsg;
//    initMsg.subject[0] = '\0';
    char timestmp[20];
    TIME.getTimestamp(timestmp);
    sprintf(initMsg.value, "%s, %s", device_id, timestmp);
    strcpy(initMsg.type, MSG_TYPE_INIT);
    strcpy(initMsg.device, DEVICE_ID);
    initMsg.retained = true;
//    initMsg.idx = 0;
    OUT.put(initMsg);
}

void scanAvailableWiFiNetworks() {
    int numberOfNetworks = WiFi.scanNetworks();
    Serial.println("---- WiFi Networks ----");
    for (int i = 0; i < numberOfNetworks; i++) {
        Serial.printf("ssid=%s, rssi=%ddb\r\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
    Serial.println("-----------------------");
}

bool wiFiReconnect() {
    if (GLOBAL.flag.wifiTryConnect) {
        Serial.printf("Connecting to %s ...\n", CTX.wifi.ssid);

        WiFi.persistent(false);
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);

        WiFi.begin(CTX.wifi.ssid, CTX.wifi.pass);
        int8_t connect_status = WiFi.waitForConnectResult();
        if (connect_status == WL_CONNECTED) {
            GLOBAL.cmd.wifiConnected = true;
            GLOBAL.status.wifiConnected = true;
            String mac = WiFi.macAddress();
            mac.replace(":", "-");
            sprintf(DEVICE_ID_MAC, "%s-%s", DEVICE_ID, mac.c_str());
            sprintf(DEVICE_ID_ADDR, "%s-%s", DEVICE_ID, CTX.mqtt.address);
            Serial.println("WiFi connected");
        } else {
            GLOBAL.status.wifiConnected = false;
            if (connect_status == WL_NO_SSID_AVAIL) {
                Serial.printf("WiFi failed: No SSID available: %s\n", CTX.wifi.ssid);
                scanAvailableWiFiNetworks();
            } else {
                Serial.printf("WiFi connect FAILED: %d\n", connect_status);
            }
        }
        return GLOBAL.cmd.wifiConnected;
    } else {
        return false;
    }
}

void processAT(const char *at_cmd) {
    const char **processed = AT.process(at_cmd);
    if (processed != NULL) {
        Serial.printf("%s%s%s\r\n", processed[0], processed[1], processed[2]);
    }
}

bool processDeviceId() {
    while (Serial.available())
        Serial.read();
    Serial.println("get_ver");
    Serial.flush();
    String resp = Serial.readStringUntil('\n');
    if (resp.length() > 0 && resp.startsWith("get_ver") && resp.indexOf('>') > 0) {
        String ver = resp.substring(resp.indexOf('>') + 2);
        ver.replace(' ', '_');
        ver.replace('+', '_');
        ver.replace('#', '_');
        ver.replace(':', '_');
        ver.replace('\r', '\0');
        strncpy(CTX.uart.device_id, ver.c_str(), CTX_LEN_DEVICE_ID);
        Serial.printf("PWR connected: %s\r\n", CTX.uart.device_id);
        return true;
    }
    return false;
}

void processCommand() {

    if (GLOBAL.flag.master && GLOBAL.status.wifiConnected && !GLOBAL.status.uartConnected && GLOBAL.status.timer_5s) {
        GLOBAL.status.uartConnected = processDeviceId();
        GLOBAL.cmd.uartConnected = GLOBAL.status.uartConnected;
        return;
    }

    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.replace('\n', '0');
        if (cmd.startsWith("AT")) {
            processAT(cmd.c_str());
        } else {
            pwrProcessor.processOut(cmd);
        }
    }

    pwrProcessor.processIn();
}

void processInternal() {
    if (!IN.isEmpty() && strcmp(IN.peek()->type, MSG_TYPE_EXEC_AT) == 0) {
        ParserModel *in_at = IN.poll();
        uint8_t total_len = strlen(in_at->value);
        uint8_t cursor = 0;
        splitLines(in_at->value);
        while (cursor < total_len) {
            AT.process(in_at->value + cursor);
            cursor += strlen(in_at->value + cursor) + 1;
            while (in_at->value[cursor] == '\0' && cursor < total_len) {
                cursor++;
            }
        }
    }
    if (!IN.isEmpty() && strcmp(IN.peek()->type, MSG_TYPE_HEALTH) == 0) {
        IN.poll();
        ParserModel healthMsg;
        strcpy(healthMsg.type, MSG_TYPE_INIT);
        strcpy(healthMsg.device, DEVICE_ID);
        strcpy(healthMsg.value, "OK");
        healthMsg.retained = false;
        OUT.put(healthMsg);

    }
}

void processMqtt(PubSubClient &client) {
    if (GLOBAL.status.mqttConnected) {
        if (!GLOBAL.flag.mqttConnect) {
            GLOBAL.status.mqttConnected = false;
            Serial.println("Disconnected from MQTT server");
            client.disconnect();
            return;
        }
        if (GLOBAL.cmd.subscribe) {
            mqttSubscribe(client);
            GLOBAL.cmd.subscribe = false;
        }
        if (GLOBAL.cmd.initiate) {
            sendInitMessage(CTX.uart.device_id);
            GLOBAL.cmd.initiate = false;
        }
        client.loop();
        mqttProcessor.publish();
    } else {
        if (GLOBAL.flag.mqttConnect && GLOBAL.status.timer_5s)
            mqttTryConnect(client);
    }
}

void mqttSubscribe(PubSubClient &client) {
    client.unsubscribe(GLOBAL.topics.sub_device_topic);
    client.unsubscribe(GLOBAL.topics.sub_uart_topic);
    buildSubTopic(CTX, DEVICE_ID);
    client.unsubscribe(GLOBAL.topics.sub_device_topic);
    client.unsubscribe(GLOBAL.topics.sub_uart_topic);
    printMQTTInfo();
}

void mqttTryConnect(PubSubClient &client) {
    Serial.printf("Connecting to MQTT server %s:%s, client_id=%s, user=%s, pass=%s\n", CTX.mqtt.host, CTX.mqtt.port,
                  DEVICE_MQTT_ID, CTX.mqtt.user, CTX.mqtt.pass);
    if (client.connect(DEVICE_MQTT_ID, CTX.mqtt.user, CTX.mqtt.pass)) {
        Serial.println("Connected to MQTT server");
        GLOBAL.cmd.subscribe = true;
        GLOBAL.status.mqttConnected = true;
    } else {
        GLOBAL.status.mqttConnected = false;
        Serial.println("Could not connect to MQTT server");
    }
}

void testConnect() {
    GLOBAL.status.wifiConnected = WiFi.isConnected();
    digitalWrite(LED,
                 GLOBAL.status.wifiConnected ? GLOBAL.status.mqttConnected ? HIGH : GLOBAL.status.timer_2Hz % 2 : LOW);
    if (GLOBAL.status.wifiConnected && GLOBAL.status.timer_5s) {
        GLOBAL.status.mqttConnected = mqttClient.connected();
    }
}

void syncTimeAndStates() {
    if (GLOBAL.cmd.wifiConnected) {
        TIME.begin();
        GLOBAL.cmd.wifiConnected = false;
    }

    if (GLOBAL.status.wifiConnected && GLOBAL.status.mqttConnected) {
        if (GLOBAL.cmd.uartConnected || GLOBAL.cmd.mqttConnected) {
            GLOBAL.cmd.subscribe = true;
            GLOBAL.cmd.uartConnected = false;
            GLOBAL.cmd.mqttConnected = false;
        }
    }

    if ((!GLOBAL.status.initiated && GLOBAL.status.wifiConnected && TIME.isReady() && GLOBAL.cmd.subscribe) ||
        (GLOBAL.status.initiated && GLOBAL.cmd.subscribe)) {
        GLOBAL.status.initiated = true;
        GLOBAL.cmd.initiate = true;
    }

    if (!GLOBAL.status.wifiConnected && GLOBAL.status.initiated) {
        TIME.end();
        GLOBAL.status.initiated = false;
    }
}

void initBt() {
    Serial.printf("Testing BT connection at %ld\n", CTX.uart.speed);
    BluetoothHC05 BT(DEVICE_ID);
    BT.begin(CTX.uart.speed);
    digitalWrite(LED, BT.isInAt() ? LOW : HIGH);
    bool proceedLink = BT.isHC05inAT();
    digitalWrite(LED, proceedLink ? LOW : HIGH);
    if (proceedLink) {
        if (BT.isInMasterMode()) {
            BT.programAsSlave();
        } else {
            digitalWrite(LED, BT.programAsMaster() ? HIGH : LOW);
        }
    }
    BT.end();
    Serial.println("\r\nTesting BT end");
}

void cleanSerial() {
    while (Serial.available())
        Serial.read();
}

void processTimer() {
    GLOBAL.status.timer_5s = false;
    GLOBAL.status.timer_2_5s = false;
    unsigned long current = millis();
    if ((current - ((current < timestamp) ? (timestamp - ULONG_MAX) : timestamp)) >= 500L) {
        GLOBAL.status.timer_2Hz++;
        timestamp = current;
        GLOBAL.status.timer_5s = !(GLOBAL.status.timer_2Hz % 10);
        GLOBAL.status.timer_2_5s = !(GLOBAL.status.timer_2Hz % 5);
    }
}

void setup() {

    Serial.begin(115200);
    Serial.println();

    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    delay(10);

    loadContext();

    initBt();

    WiFi.disconnect();

    mqttClient.setServer(CTX.mqtt.host, atoi(CTX.mqtt.port));
    mqttClient.setCallback(mqtt_callback);

    Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
    Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
    Serial.println("Bridge started");
    cleanSerial();

    timestamp = millis();
}

void loop() {
    testConnect();

    processCommand();

    if (
            !GLOBAL.status.wifiConnected &&
            GLOBAL.status.timer_2_5s &&
            GLOBAL.flag.connect &&
            !wiFiReconnect()
            )
        return;

    syncTimeAndStates();

    if (GLOBAL.status.wifiConnected) {
        if (GLOBAL.flag.connect) {
            processMqtt(mqttClient);
            processInternal();
        } else {
            WiFi.disconnect();
            Serial.println("WiFi disconnected");
        }
    }

    processTimer();
}