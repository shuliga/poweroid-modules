//
// Created by SHL on 07.01.2020.
//

#ifndef POWEROID_MODULES_MQTTPROCESSOR_H
#define POWEROID_MODULES_MQTTPROCESSOR_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "CircularBuffer.h"
#include "Context.h"
#include "ParserModel.h"

class MqttProcessor {

private:
    CircularBuffer<ParserModel> *IN, *OUT;
    Context *CTX;
    PubSubClient *mqttClient;

public:

    MqttProcessor(CircularBuffer<ParserModel> *in, CircularBuffer<ParserModel> *out, Context *ctx, PubSubClient *pClient);

    void process(char *topic, unsigned char *payload, unsigned int length);

    void publish();
};


#endif //POWEROID_MODULES_MQTTPROCESSOR_H
