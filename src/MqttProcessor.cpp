//
// Created by SHL on 07.01.2020.
//

#include "MqttProcessor.h"
#include "MqttParser.h"
#include "ParserConstants.h"

MqttProcessor::MqttProcessor(CircularBuffer *in, CircularBuffer *out, Context *ctx, PubSubClient *pClient)
        : IN(in), OUT(out), CTX(ctx), mqttClient(pClient) {}

void MqttProcessor::process(char *topic, unsigned char *payload, unsigned int length) {
    char str_buffer[length + 1];
    strncpy(str_buffer, reinterpret_cast<const char *>(payload), length);
    str_buffer[length] = '\0';
    String path = String(topic);

    if(CTX->verbose){
        Serial.print(topic);
        Serial.print(" => ");
        Serial.println(str_buffer);
    }

    ParserModel msg;

    if (strncmp(CTX->sub_path, topic, strlen(CTX->sub_path)) == 0) {
        if (MQTT_Parser.parseOut(path, str_buffer, msg)){
            IN->put(msg);
        }
    }
}

void MqttProcessor::publish() {
    if (!OUT->isEmpty()) {
        ParserModel msg = OUT->poll();

        if (&msg != NULL) {
            char target_topic[64];
            char path[64];
            char payload[MODEL_VAL_LENGTH];

            if (MQTT_Parser.parseIn(msg, path, payload)) {
                sprintf(target_topic, "%s/%s", CTX->pub_topic, path);

                if(CTX->verbose){
                    Serial.print("Sending OUT msg: ");
                    Serial.print(target_topic);
                    Serial.print(" / ");
                    Serial.println(payload);
                }

                mqttClient->publish(target_topic, payload, strcmp(msg.type, MSG_TYPE_STATUS) == 0);
            }
        }
    }
}

