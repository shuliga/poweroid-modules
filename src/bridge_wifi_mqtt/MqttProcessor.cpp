//
// Created by SHL on 07.01.2020.
//

#include "MqttProcessor.h"
#include "MqttParser.h"
#include "ParserConstants.h"
#include "Common.h"
#include "Global.h"

static const long utcOffsetInSeconds = 3600;

MqttProcessor::MqttProcessor(CircularBuffer<ParserModel> *in, CircularBuffer<ParserModel> *out, Context *ctx, PubSubClient *pClient)
        : IN(in), OUT(out), CTX(ctx), mqttClient(pClient) {
}

void MqttProcessor::process(char *topic, unsigned char *payload, unsigned int length) {
    char str_buffer[length + 1];
    strncpy(str_buffer, reinterpret_cast<const char *>(payload), length);
    str_buffer[length] = '\0';
    String path = String(topic);
    logToSerial(topic, " => ", str_buffer);

    ParserModel msg;
    if (strncmp(GLOBAL.topics.prefix, topic, strlen(GLOBAL.topics.prefix)) == 0) {
        if (MQTT_Parser.parseOut(path, str_buffer, msg)){
            IN->put(msg);
        }
    }
}

void MqttProcessor::publish() {
    if (!OUT->isEmpty()) {
        ParserModel* msg = OUT->poll();

        if (msg) {
            char target_topic[GLOBAL_LEN_TOPIC];
            char path[GLOBAL_LEN_TOPIC];
            char payload[MODEL_VAL_LENGTH];

            if (MQTT_Parser.parseIn(*msg, path, payload)) {
                buildPubTopic(*CTX, msg->device);
                sprintf(target_topic, "%s/%s", GLOBAL.topics.pub_topic, path);

                logToSerial("MQTT: sending OUT msg: ", target_topic, " | ", payload);
                logToSerial("MQTT: payload length: ", strlen(payload));
                mqttClient->publish(target_topic, payload, msg->retained);
            }
        }
    }
}

