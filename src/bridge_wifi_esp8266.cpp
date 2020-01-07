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

#define VERSION "PWM-BWM-01"

Context CTX;

CircularBuffer IN, OUT;

WiFiClient wclient;
PubSubClient mqttClient(wclient);

AtCommands AT(&CTX);
PoweroidProcessor pwrProcessor(&IN, &OUT, &CTX);
MqttProcessor mqttProcessor(&IN, &OUT, &CTX, &mqttClient);

void sendInitMessage();

void mqtt_callback(char *topic, unsigned char *payload, unsigned int length){
    mqttProcessor.process(topic, payload, length);
}

void setup() {
    Serial.begin(CTX.uart.speed);
    Serial.println();
    Serial.println("Loading context");
    loadContext(CTX);
    strcpy(CTX.device_id, "PWR-thermo_2x3");
    buildPubTopic(CTX);
    buildSubTopic(CTX);
    delay(10);
    Serial.print("MQTT pub: ");
    Serial.println(CTX.pub_topic);
    Serial.print("MQTT sub RAW: ");
    Serial.println(CTX.sub_topic_raw);
    Serial.print("MQTT sub: ");
    Serial.println(CTX.sub_topic);

    mqttClient.setServer(CTX.mqtt.host, atoi(CTX.mqtt.port));
    mqttClient.setCallback(mqtt_callback);
    Serial.println("Bridge started.");
    sendInitMessage();
}

void sendInitMessage() {
    ParserModel initMsg;
    initMsg.subject[0] ='\0';
    strcpy(initMsg.value, VERSION);
    strcpy(initMsg.type, MSG_TYPE_INIT);
    initMsg.idx = 0;
    OUT.put(initMsg);
}

bool wiFiReconnect() {
    Serial.print("Connecting to ");
    Serial.print(CTX.wifi.ssid);
    Serial.println("...");
    WiFi.begin(CTX.wifi.ssid, CTX.wifi.pass);
    bool connected = WiFi.waitForConnectResult() == WL_CONNECTED;
    if (connected)
        Serial.println("WiFi connected");
    return connected;
}

void processAT(const char *at_cmd) {
    const char *processed = AT.process(at_cmd);
    if (processed != NULL) {
        Serial.println(processed);
        if (AT.strEndsWith(at_cmd, INFO)) {
            delete[] processed;
        }
    }
}

void processCommand() {
    if (Serial.available()){
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
        Serial.println("Connecting to MQTT server...");
        if (client.connect(CTX.device_id, CTX.mqtt.user, CTX.mqtt.pass)) {
            Serial.println("Connected to MQTT server");
            client.subscribe(CTX.sub_topic);
            client.subscribe(CTX.sub_topic_raw);
        } else {
            Serial.println("Could not connect to MQTT server");
        }
    }

    if (client.connected()) {
        client.loop();
        mqttProcessor.publish();
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        if (!wiFiReconnect())
            return;
    }

    processCommand();

    if (WiFi.status() == WL_CONNECTED) {
        processMqtt(mqttClient);
    }

}
