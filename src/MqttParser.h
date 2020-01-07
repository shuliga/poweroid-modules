//
// Created by SHL on 01.01.2020.
//

#ifndef POWEROID_MODULES_MQTTPARSER_H
#define POWEROID_MODULES_MQTTPARSER_H

#include "ParserModel.h"

class MqttParser {
public:
    bool parseOut(String &path, char *payload, ParserModel &parsed);
    bool parseIn(ParserModel &input, char *parsedPath, char *payload);

    char *getItemBackwards(char *split, uint8_t size, uint8_t idx);

    uint8_t splitStr(char *split, const char *src, const char divider);
};

extern MqttParser MQTT_Parser;

#endif //POWEROID_MODULES_MQTTPARSER_H
