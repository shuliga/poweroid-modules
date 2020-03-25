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
char DEVICE_ID_MAC[34];

unsigned long timestamp = 0;

Context CTX;

CircularBuffer IN, OUT;

WiFiClient wclient;
PubSubClient mqttClient(wclient);

AtCommands AT(&CTX);
PoweroidProcessor pwrProcessor(&IN, &OUT, &CTX);
MqttProcessor mqttProcessor(&IN, &OUT, &CTX, &mqttClient);

void sendInitMessage();

void printMQTTInfo();

void testConnect();

void loadContext();

void syncTimeAndStates();

void initBt();

void cleanSerial();

void processTimer();

void mqttTryConnect(PubSubClient &client);

void mqttSubscribe(PubSubClient &client);

void mqtt_callback(char *topic, unsigned char *payload, unsigned int length) {
    mqttProcessor.process(topic, payload, length);
}

void loadContext() {
    Serial.println("Loading context");
    loadContext(CTX);
    buildSubTopic(CTX);
}

void printMQTTInfo() {
    Serial.printf("MQTT sub: %s\r\n", CTX.sub_topic);
    Serial.printf("MQTT sub RAW: %s\r\n", CTX.sub_topic_raw);
}

void sendInitMessage(const char * device_id) {
    ParserModel initMsg;
    initMsg.subject[0] = '\0';
    char timestmp[20];
    TIME.getTimestamp(timestmp);
    sprintf(initMsg.value, "%s, %s", device_id, timestmp);
    strcpy(initMsg.type, MSG_TYPE_INIT);
    strcpy(initMsg.device, DEVICE_ID);
    initMsg.idx = 0;
    OUT.put(initMsg);
}

bool wiFiReconnect() {
    printToSerial("Connecting to ", CTX.wifi.ssid, "...");
    WiFi.begin(CTX.wifi.ssid, CTX.wifi.pass);
    bool connected = WiFi.waitForConnectResult();
    if (connected){
        GLOBAL.cmd.wifiConnected = true;
        GLOBAL.status.wifiConnected = true;
        String mac = WiFi.macAddress();
        mac.replace(":","-");
        sprintf(DEVICE_ID_MAC, "%s-%s", DEVICE_ID, mac.c_str());
        printToSerial("WiFi connected");
    } else {
        GLOBAL.status.wifiConnected = false;
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

bool processDeviceId() {
    Serial.println("get_ver");
    Serial.flush();
    delay(350);
    String resp = Serial.readStringUntil('\n');
    while (Serial.available())
        Serial.read();
    if(resp.length() > 0){
        String ver = resp.substring(resp.indexOf('>') + 2);
        ver.replace(' ','_');
        ver.replace('+','_');
        ver.replace('#','_');
        ver.replace(':','_');
        ver.replace('\r','\0');
        strncpy(CTX.uart.device_id, ver.c_str(), CTX_LEN_DEVICE_ID);
        Serial.printf("PWR connected: %s\r\n", CTX.uart.device_id);
        return true;
    }
    return false;
}

void processCommand() {
    if (GLOBAL.flag.master && GLOBAL.status.wifiConnected && !GLOBAL.status.uartConnected && GLOBAL.status.timer_5s){
        GLOBAL.status.uartConnected = processDeviceId();
        GLOBAL.cmd.uartConnected = GLOBAL.status.uartConnected;
        return;
    }
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

void mqttProcess(PubSubClient &client) {
    if (GLOBAL.status.mqttConnected) {
        if (GLOBAL.cmd.subscribe) {
            mqttSubscribe(client);
            GLOBAL.cmd.subscribe = false;
        }
        if (GLOBAL.cmd.initiate){
            sendInitMessage(CTX.uart.device_id);
            GLOBAL.cmd.initiate = false;
        }
        client.loop();
        mqttProcessor.publish();
    } else {
        if (GLOBAL.status.timer_5s)
            mqttTryConnect(client);
    }
}

void mqttSubscribe(PubSubClient &client) {
    client.unsubscribe(CTX.sub_topic);
    client.unsubscribe(CTX.sub_topic_raw);
    buildSubTopic(CTX);
    client.subscribe(CTX.sub_topic);
    client.subscribe(CTX.sub_topic_raw);
    printMQTTInfo();
}

void mqttTryConnect(PubSubClient &client) {
    Serial.printf("Connecting to MQTT server %s:%s, device=%s, user=%s, pass=%s\n", CTX.mqtt.host, CTX.mqtt.port, DEVICE_ID_MAC, CTX.mqtt.user, CTX.mqtt.pass);
    if (client.connect(DEVICE_ID_MAC, CTX.mqtt.user, CTX.mqtt.pass)) {
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
    digitalWrite(LED, GLOBAL.status.wifiConnected ? GLOBAL.status.mqttConnected ? HIGH : GLOBAL.status.timer_2Hz % 2 : LOW);
    if (GLOBAL.status.wifiConnected && GLOBAL.status.timer_5s) {
        GLOBAL.status.mqttConnected = mqttClient.connected();
    }
}

void syncTimeAndStates() {
    if (GLOBAL.cmd.wifiConnected) {
        TIME.begin();
        GLOBAL.cmd.wifiConnected = false;
    }

    if(GLOBAL.status.wifiConnected && GLOBAL.status.mqttConnected){
        if(GLOBAL.cmd.uartConnected || GLOBAL.cmd.mqttConnected) {
            GLOBAL.cmd.subscribe = true;
            GLOBAL.cmd.uartConnected = false;
            GLOBAL.cmd.mqttConnected = false;
        }
    }

    if ((!GLOBAL.status.initiated && GLOBAL.status.wifiConnected && TIME.isReady() && GLOBAL.cmd.subscribe) || (GLOBAL.status.initiated && GLOBAL.cmd.subscribe)) {
        GLOBAL.status.initiated = true;
        GLOBAL.cmd.initiate = true;
    }

    if (!GLOBAL.status.wifiConnected && GLOBAL.status.initiated) {
        TIME.end();
        GLOBAL.status.initiated = false;
    }
}

void initBt() {
    printToSerial("Testing BT connection at ", CTX.uart.speed);
    BluetoothHC05 BT(DEVICE_ID);
    BT.begin(CTX.uart.speed);
    digitalWrite(LED, BT.isInAt() ? LOW : HIGH);
    bool proceedLink = BT.isHC05inAT();
    digitalWrite(LED, proceedLink ? LOW : HIGH);
    if (proceedLink){
        if(BT.isInMasterMode()){
            BT.programAsSlave();
        } else {
            digitalWrite(LED, BT.programAsMaster() ? HIGH : LOW);
        }
    }
    BT.end();
    printToSerial("\r\nTesting BT end");
}

void cleanSerial() {
    while (Serial.available())
        Serial.read();
}

void processTimer() {
    GLOBAL.status.timer_5s = false;
    unsigned long current = millis();
    if((current - ((current < timestamp) ? (timestamp - ULONG_MAX) : timestamp))  >= 500L){
        GLOBAL.status.timer_2Hz++;
        timestamp = current;
        GLOBAL.status.timer_5s = !(GLOBAL.status.timer_2Hz % 10);
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
    WiFi.setAutoReconnect(true);

    mqttClient.setServer(CTX.mqtt.host, atoi(CTX.mqtt.port));
    mqttClient.setCallback(mqtt_callback);

    Serial.println("Bridge started.");
    cleanSerial();

    timestamp = millis();
}

void loop() {
    testConnect();

    if (!GLOBAL.status.wifiConnected && GLOBAL.flag.connect && !wiFiReconnect())
        return;

    processCommand();

    syncTimeAndStates();

    if (GLOBAL.status.wifiConnected) {
        if (GLOBAL.flag.connect) {
            mqttProcess(mqttClient);
        } else {
            WiFi.disconnect();
            printToSerial("WiFi disconnected");
        }
    }

    processTimer();
}