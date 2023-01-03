//
// Created by SHL on 23.12.2019.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Global.h"
#include "Context.h"
#include "CircularBuffer.h"
#include "AtCommands.h"
#include "OTA.h"
#include "PoweroidProcessor.h"
#include "MqttProcessor.h"
#include "ParserConstants.h"
#include "Common.h"
#include "BluetoothHC05.h"
#include "TimeClient.h"

#define LED 2

const char *MODULE_ID = "PWM-BMU-01"; // Poweroid module, Bridge MQTT-UART
char DEVICE_ID_MAC[34];
char DEVICE_ID_ADDR[64];

char *DEVICE_MQTT_ID = DEVICE_ID_MAC;

unsigned long timestamp = 0;

Context CTX;

CircularBuffer<ParserModel> IN, OUT;

WiFiClient wclient;
WiFiClientSecure wsclient;
PubSubClient mqttClient(wclient);

OTA ota(wsclient);

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

void mqtt_callback(char *topic, uint8_t *payload, unsigned int length) {
    mqttProcessor.process(topic, payload, length);
}

void loadContext() {
    Serial.println("Loading context");
    loadContext(CTX);
    buildSubTopic(CTX, MODULE_ID);
}

void printMQTTInfo() {
    Serial.printf("Subscribed to MQTT UART device path: %s\r\n", GLOBAL.topics.sub_uart_device_topic);
    Serial.printf("Subscribed to MQTT Bridge device path: %s\r\n", GLOBAL.topics.sub_module_topic);
}

void sendInitMessage(const char *device_id) {
    ParserModel initMsg;
    char timestmp[20];
    TIME.getTimestamp(timestmp);
    sprintf(initMsg.value, "%s, %s", device_id, timestmp);
    strcpy(initMsg.mode, MSG_TYPE_INIT);
    strcpy(initMsg.device, MODULE_ID);
    initMsg.retained = true;
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
            GLOBAL.cmd.wifiConnect = true;
            GLOBAL.status.wifiConnected = true;
            String mac = WiFi.macAddress();
            mac.replace(":", "-");
            sprintf(DEVICE_ID_MAC, "%s-%s", MODULE_ID, mac.c_str());
            sprintf(DEVICE_ID_ADDR, "%s-%s", MODULE_ID, CTX.mqtt.address);
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
        return GLOBAL.cmd.wifiConnect;
    } else {
        return false;
    }
}

bool processToken(uint8_t token) {
    while (Serial.available())
        Serial.read();
    Serial.print("pass_token:");
    Serial.println(token);
    Serial.flush();
    String resp = Serial.readStringUntil('\n');
    if (resp.length() > 0 && resp.startsWith("pass_token") && resp.indexOf('>') > 0) {
        return true;
    }
    return false;
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
        String ver = "";
        if (CTX.uart.token > 0){
            ver.concat(CTX.uart.token);
            ver.concat('/');
        }
        ver.concat(resp.substring(resp.indexOf('>') + 2));
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

void processUartDevice() {
    if (GLOBAL.cmd.updateToken && GLOBAL.status.timer_2_5s) {
        GLOBAL.status.uartConnected = false;
        GLOBAL.flag.master = false;
        if (CTX.uart.token == 0) {
            processToken(CTX.uart.token);
            GLOBAL.cmd.updateToken = false;
            GLOBAL.flag.master = true;
            resetUartDeviceId(CTX);
            GLOBAL.cmd.uartConnect = true;
            return;
        } else {
            if(processToken(CTX.uart.token)){
                GLOBAL.cmd.updateToken = false;
                GLOBAL.flag.master = true;
            }
        }
    }
    if (GLOBAL.flag.master && GLOBAL.status.wifiConnected && !GLOBAL.status.uartConnected && GLOBAL.status.timer_5s) {
        GLOBAL.status.uartConnected = processDeviceId();
        GLOBAL.cmd.uartConnect = GLOBAL.status.uartConnected;
        return;
    }
}


void processCommand() {

    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.startsWith("AT")) {
            logToSerial("Processing AT: ", cmd.c_str());
            processAT(cmd.c_str());
        } else {
            logToSerial("Processing PWR: ", cmd.c_str());
            pwrProcessor.processOut(cmd);
        }
    }

    pwrProcessor.processIn();
}

void processInternal() {
    if (!IN.isEmpty() && strcmp(IN.peek()->mode, MSG_MODE_EXEC_AT) == 0) {
        ParserModel *atIn = IN.poll();
        uint8_t total_len = strlen(atIn->value);
        uint8_t cursor = 0;
        splitLines(atIn->value);
        while (cursor < total_len) {
            AT.process(atIn->value + cursor);
            cursor += strlen(atIn->value + cursor) + 1;
            while (atIn->value[cursor] == '\0' && cursor < total_len) {
                cursor++;
            }
        }
        return;
    }
    if (!IN.isEmpty() && strcmp(IN.peek()->mode, MSG_MODE_HEALTH) == 0) {
        ParserModel healthMsg;
        strcpy(healthMsg.mode, MSG_MODE_HEALTH );
        strcpy(healthMsg.subject, SUBJ_RESPONSE);
        strcpy(healthMsg.device, MODULE_ID);
        strcpy(healthMsg.value, "Health - OK");
        healthMsg.retained = false;
        OUT.put(healthMsg);
        return;
    }

    if (!IN.isEmpty() && strcmp(IN.peek()->mode, MSG_MODE_OTA) == 0) {
        ParserModel *otaIn = IN.poll();
        const char * res = ota.install(otaIn->value);
        ParserModel otaMsg;
        strcpy(otaMsg.mode, MSG_MODE_OTA);
        strcpy(otaMsg.device, MODULE_ID);
        strcpy(otaMsg.subject, SUBJ_RESPONSE);
        strcpy(otaMsg.value, res);
        otaMsg.retained = false;
        OUT.put(otaMsg);
        if (strcmp(res, "OK") == 0){
            ota.apply();
        }
        return;
    }
}

void processMqtt(PubSubClient &client) {
    if (GLOBAL.status.mqttConnected) {
        if (!GLOBAL.flag.mqttConnect) {
            client.disconnect();
            client.loop();
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
        mqttProcessor.publish();
        if (GLOBAL.status.timer_0_5s){
            client.loop();
        }
    } else {
        if (GLOBAL.flag.mqttConnect && GLOBAL.status.timer_5s)
            mqttTryConnect(client);
    }
}

void mqttSubscribe(PubSubClient &client) {
    client.unsubscribe(GLOBAL.topics.sub_module_topic);
    client.unsubscribe(GLOBAL.topics.sub_uart_device_topic);
    buildSubTopic(CTX, MODULE_ID);
    client.subscribe(GLOBAL.topics.sub_module_topic);
    client.subscribe(GLOBAL.topics.sub_uart_device_topic);
    printMQTTInfo();
}

void mqttTryConnect(PubSubClient &client) {
    Serial.printf("Connecting to MQTT server %s:%s, client_id=%s, user=%s, pass=%s\n", CTX.mqtt.host, CTX.mqtt.port,
                  DEVICE_MQTT_ID, CTX.mqtt.user, CTX.mqtt.pass);
    if (uint8_t status = client.connect(DEVICE_MQTT_ID, CTX.mqtt.user, CTX.mqtt.pass)) {
        Serial.println("Connected to MQTT server");
        GLOBAL.cmd.subscribe = true;
        GLOBAL.status.mqttConnected = true;
        GLOBAL.status.mqttDisconnectLatch = true;
    } else {
        GLOBAL.status.mqttConnected = false;
        Serial.printf("Could not connect to MQTT server: %d\n", status);
    }
}

void testConnect() {
    GLOBAL.status.wifiConnected = WiFi.isConnected();
    digitalWrite(LED,GLOBAL.status.wifiConnected ? GLOBAL.status.mqttConnected ? HIGH : GLOBAL.status.counter_2Hz % 2 : LOW);
    if (GLOBAL.status.wifiConnected && GLOBAL.status.timer_5s) {
        GLOBAL.status.mqttConnected = mqttClient.connected();
        if (!GLOBAL.status.mqttConnected && GLOBAL.status.mqttDisconnectLatch){
            Serial.println("Disconnected from MQTT server");
            GLOBAL.status.mqttDisconnectLatch = false;
        }
    }
}

void syncTimeAndStates() {
    if (GLOBAL.cmd.wifiConnect) {
        TIME.begin();
        GLOBAL.cmd.wifiConnect = false;
    }

    if (GLOBAL.status.wifiConnected && GLOBAL.status.mqttConnected) {
        if (GLOBAL.cmd.uartConnect || GLOBAL.cmd.mqttConnect) {
            GLOBAL.cmd.subscribe = true;
            GLOBAL.cmd.uartConnect = false;
            GLOBAL.cmd.mqttConnect = false;
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
    BluetoothHC05 BT(MODULE_ID);
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
    GLOBAL.status.timer_0_5s = false;
    unsigned long current = millis();
    if ((current - ((current < timestamp) ? (timestamp - ULONG_MAX) : timestamp)) >= 500L) {
        GLOBAL.status.counter_2Hz++;
        timestamp = current;
        GLOBAL.status.timer_5s = !(GLOBAL.status.counter_2Hz % 10);
        GLOBAL.status.timer_2_5s = !(GLOBAL.status.counter_2Hz % 5);
        GLOBAL.status.timer_0_5s = true;
    }
}

void setup() {
    ESP.wdtDisable();

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

    Serial.printf("Flash size: %u\n", ESP.getFlashChipRealSize());
    Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
    Serial.printf("Free sketch space: %u\n", ESP.getFreeSketchSpace());
    Serial.println("Bridge started");
    cleanSerial();

    timestamp = millis();
    ESP.wdtEnable(WDTO_8S);
}

void loop() {
    testConnect();
    processUartDevice();
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

    ESP.wdtFeed();
}