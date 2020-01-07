//
// Created by SHL on 01.01.2020.
//

#include "ParserConstants.h"
#include "MqttParser.h"

static const char cmd_divider = '/';

bool MqttParser::parseOut(String &path, char * payload, ParserModel &parsed) {
    uint8_t idx_start = path.lastIndexOf(cmd_divider) + 1;
    if (idx_start > 0) {
        char split[128];
        char subject[MODEL_SUBJ_LENGTH] = "";
        char type[MODEL_TYPE_LENGTH];
        splitStr(split, path.c_str(), cmd_divider);
        strncpy(type, getItemBackwards(split, path.length(), 1), MODEL_TYPE_LENGTH);
        strncpy(parsed.value, payload, MODEL_VAL_LENGTH);
        if (strcmp(type, MSG_TYPE_IN) != 0){
            strncpy(parsed.type, MSG_TYPE_CMD, MODEL_TYPE_LENGTH);
            strncpy(parsed.mode, getItemBackwards(split, path.length(),3), MODEL_MODE_LENGTH);
            strncpy(subject, getItemBackwards(split, path.length(), 2), MODEL_SUBJ_LENGTH);
            if (strcmp(subject, SUBJ_PROP) == 0) {
                strcpy(parsed.subject, SUBJ_PROP);
                parsed.idx = path.substring(idx_start).toInt();
                return true;
            }
            if (strcmp(subject, SUBJ_STATE) == 0) {
                strcpy(parsed.subject, SUBJ_STATE);
                parsed.idx = path.substring(idx_start).toInt();
                return true;
            }
        } else{
            strncpy(parsed.type, type, MODEL_TYPE_LENGTH);
            return true;
        }
    }
    return false;
}

bool MqttParser::parseIn(ParserModel &input, char * parsedPath, char * payload) {
    strcpy(payload, input.value);
    if (strcmp(input.type, MSG_TYPE_STATUS) == 0){
        sprintf(parsedPath, "%s/%s/%d", input.type, input.subject, input.idx);
        return true;
    }
    sprintf(parsedPath, "%s", input.type);
    return true;
}

uint8_t MqttParser::splitStr(char * split, const char * src, const char divider){
    uint8_t z = 1;
    strcpy(split, src);
    for(uint8_t i = 0; i < strlen(src); i++){
        if ( split[i] == divider) {
            split[i] ='\0';
            z++;
        }
    }
    return z;
}

char * MqttParser::getItemBackwards(char * split, uint8_t size, uint8_t idx){
    uint8_t z = 0;
    for(uint8_t i = size - 1; i >=0; i--){
        if ( split[i] == '\0' || i == 0) {
            z++;
        }
        if (z == idx) {
            return &split[i] + (i == 0 ? 0 : 1);
        }
    }
    return NULL;
}

MqttParser MQTT_Parser;
