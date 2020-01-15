//
// Created by SHL on 07.01.2020.
//

#include "MqttProcessor.h"
#include "MqttParser.h"
#include "ParserConstants.h"
#include "Common.h"

static const long utcOffsetInSeconds = 3600;

MqttProcessor::MqttProcessor(CircularBuffer *in, CircularBuffer *out, Context *ctx, PubSubClient *pClient)
        : IN(in), OUT(out), CTX(ctx), mqttClient(pClient) {
}

void MqttProcessor::process(char *topic, unsigned char *payload, unsigned int length) {
    char str_buffer[length + 1];
    strncpy(str_buffer, reinterpret_cast<const char *>(payload), length);
    str_buffer[length] = '\0';
    String path = String(topic);
    printToSerial(topic, " => ", str_buffer);

    ParserModel msg;
    if (strncmp(CTX->sub_path, topic, strlen(CTX->sub_path)) == 0) {
        if (MQTT_Parser.parseOut(path, str_buffer, msg)){
            IN->put(msg);
        }
    }
}

void MqttProcessor::publish() {
    if (!OUT->isEmpty()) {
        ParserModel* msg = OUT->poll();

        if (msg) {
            char target_topic[CTX_LEN_TOPIC];
            char path[CTX_LEN_TOPIC];
            char payload[MODEL_VAL_LENGTH];

            if (MQTT_Parser.parseIn(*msg, path, payload)) {
                sprintf(target_topic, "%s/%s", CTX->pub_topic, path);

                printToSerial("Sending OUT msg: ", target_topic, " | ", payload);
                mqttClient->publish(target_topic, payload, strcmp(msg->type, MSG_TYPE_STATUS) == 0);
            }
        }
    }
}

