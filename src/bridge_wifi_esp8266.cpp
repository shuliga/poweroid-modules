//
// Created by SHL on 23.12.2019.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char * ssid = "SHL-Net-2";
const char * pass = "76543210";

const char * mqtt_server = "server";
const int mqtt_port = 11140;
const char * mqtt_user = "SHL-Net-2";
const char * mqtt_pass = "76543210";


void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print(topic);
    Serial.print(" => ");
    String payload_str = String((char*)payload);
    Serial.println(payload_str);


    if(strcmp(topic, "test/led") == 0)
    {
        int stled = payload_str.toInt();
        digitalWrite(5,stled);
    }
}

WiFiClient wclient;
PubSubClient client(wclient);

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();
    pinMode(5, OUTPUT);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting to ");
        Serial.print(ssid);
        Serial.println("...");
        WiFi.begin(ssid, pass);

        if (WiFi.waitForConnectResult() != WL_CONNECTED)
            return;
        Serial.println("WiFi connected");
    }

    if (WiFi.status() == WL_CONNECTED) {
        if (!client.connected()) {
            Serial.println("Connecting to MQTT server");
            if (client.connect("arduinoClient2", mqtt_user, mqtt_pass)) {
                Serial.println("Connected to MQTT server");
                client.subscribe("test/led");
            } else {
                Serial.println("Could not connect to MQTT server");
            }
        }

        if (client.connected()){
            client.loop();
//            TempSend();
        }

    }
}