//
// Created by SHL on 23.12.2019.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
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

Context CTX;

CircularBuffer IN, OUT;

WiFiClient wclient;
PubSubClient mqttClient(wclient);

AtCommands AT(&CTX);
PoweroidProcessor pwrProcessor(&IN, &OUT, &CTX);
MqttProcessor mqttProcessor(&IN, &OUT, &CTX, &mqttClient);

bool initiated = false;

void sendInitMessage();

void printInfo();

void testWiFiConnect();

void loadContext();

void timeSync(bool connected);

void initBt();

void cleanSerial();

void mqtt_callback(char *topic, unsigned char *payload, unsigned int length) {
    mqttProcessor.process(topic, payload, length);
}

void loadContext() {
    Serial.println("Loading context");
    loadContext(CTX);
    buildPubTopic(CTX);
    buildSubTopic(CTX);
}

void printInfo() {
    Serial.print("MQTT pub: ");
    Serial.println(CTX.pub_topic);
    Serial.print("MQTT sub: ");
    Serial.println(CTX.sub_topic);
    Serial.print("MQTT sub RAW: ");
    Serial.println(CTX.sub_topic_raw);
}

void sendInitMessage() {
    ParserModel initMsg;
    initMsg.subject[0] = '\0';
    char timestamp[20];
    TIME.getTimestamp(timestamp);
    sprintf(initMsg.value, "%s,%s", DEVICE_ID, timestamp);
    strcpy(initMsg.type, MSG_TYPE_INIT);
    initMsg.idx = 0;
    OUT.put(initMsg);
}

bool wiFiReconnect() {
    printToSerial("Connecting to ", CTX.wifi.ssid, "...");
    WiFi.begin(CTX.wifi.ssid, CTX.wifi.pass);
    bool connected = WiFi.waitForConnectResult() == WL_CONNECTED;
    if (connected){
        CTX.wifi.firstConnected = true;
        printToSerial("WiFi connected");
    }
    return connected;
}

void processAT(const char *at_cmd) {
    const char *processed = AT.process(at_cmd);
    if (processed != NULL) {
        Serial.println(processed);
        if (strEndsWith(at_cmd, INFO)) {
            delete[] processed;
        }
    }
}

void processCommand() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd.startsWith("AT")) {
            processAT(cmd.c_str());
        } else {
            pwrProcessor.processOut(cmd);
        }
    }
    pwrProcessor.processIn();
}

void processMqtt(PubSubClient &client) {
    if (!client.connected()) {
        printToSerial("Connecting to MQTT server...");
        if (client.connect(CTX.device_id, CTX.mqtt.user, CTX.mqtt.pass)) {
            Serial.println("Connected to MQTT server");
            client.subscribe(CTX.sub_topic);
            client.subscribe(CTX.sub_topic_raw);
        } else {
            printToSerial("Could not connect to MQTT server");
        }
    }

    if (client.connected()) {
        client.loop();
        mqttProcessor.publish();
    }
}

void testWiFiConnect() {
    CTX.wifi.connected = WiFi.status() == WL_CONNECTED;
//    digitalWrite(LED, CTX.wifi.connected ? HIGH : LOW);
}

void timeSync(bool connected) {
    if (CTX.wifi.firstConnected) {
        TIME.begin();
        CTX.wifi.firstConnected = false;
    }
    if (connected && !initiated && TIME.isReady()) {
        sendInitMessage();
        initiated = true;
    }
    if (!connected && initiated) {
        TIME.end();
        initiated = false;
    }
}

void initBt() {
    printToSerial("Testing BT connection");
    BluetoothHC05 BT(DEVICE_ID);
    BT.begin(CTX.uart.speed);
//    bool proceedLink = BT.isHC05inAT();
//    digitalWrite(LED, proceedLink ? LOW : HIGH);
//    if (proceedLink){
//        printToSerial("Connected BT module in AT mode found.\n\rPairing...");
//    }
    BT.end();
}

void cleanSerial() {
    while (Serial.available())
        Serial.read();
}

void setup() {

    Serial.begin(CTX.uart.speed);
    Serial.println();

    pinMode(LED, OUTPUT);
    delay(10);

    loadContext();

    initBt();

    mqttClient.setServer(CTX.mqtt.host, atoi(CTX.mqtt.port));
    mqttClient.setCallback(mqtt_callback);

    printInfo();

    Serial.println("Bridge started.");

    cleanSerial();
}

void loop() {
    testWiFiConnect();

    timeSync(CTX.wifi.connected);

    if (!CTX.wifi.connected && GLOBAL.connect && !wiFiReconnect())
        return;

    processCommand();

    if (CTX.wifi.connected) {
        if (!GLOBAL.connect) {
            WiFi.disconnect();
            printToSerial("WiFi disconnected");
        } else {
            processMqtt(mqttClient);
        }
    }

}

