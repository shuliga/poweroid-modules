//
// Created by SHL on 01.01.2020.
//

#include "ParserConstants.h"
#include "Common.h"
#include "MqttParser.h"

static const char cmd_divider = '/';

bool MqttParser::parseOut(String &path, char *payload, ParserModel &parsed) {
    uint8_t idx_start = path.lastIndexOf(cmd_divider) + 1;
    if (idx_start > 0) {
        char split[128];
        char subject[MODEL_SUBJ_LENGTH] = "";
        char type[MODEL_TYPE_LENGTH];
        uint8_t sections = splitStr(split, path.c_str(), cmd_divider);
        if (sections > 4 ) {
            strncpy(type, getItemBackwards(split, path.length(), 1), MODEL_TYPE_LENGTH);
            strncpy(parsed.value, payload, MODEL_VAL_LENGTH);
            if (strcmp(type, MSG_TYPE_RAW_IN) == 0) {
                strncpy(parsed.type, type, MODEL_TYPE_LENGTH);
                return true;
            }
            if (strcmp(type, MSG_TYPE_EXEC_AT) == 0) {
                strncpy(parsed.type, type, MODEL_TYPE_LENGTH);
                return true;
            }
            if (strcmp(type, MSG_TYPE_HEALTH) == 0) {
                strncpy(parsed.type, type, MODEL_TYPE_LENGTH);
                return true;
            }
            if (strcmp(type, MSG_TYPE_OTA) == 0) {
                strncpy(parsed.type, type, MODEL_TYPE_LENGTH);
                return true;
            }
            strncpy(type, getItem(split, path.length(), 5), MODEL_TYPE_LENGTH);
            if (strcmp(type, MSG_TYPE_CMD) == 0) {
                strncpy(parsed.type, MSG_TYPE_CMD, MODEL_TYPE_LENGTH);
                strncpy(subject, getItemBackwards(split, path.length(), 2), MODEL_SUBJ_LENGTH);
                strncpy(parsed.action, getItemBackwards(split, path.length(), 3), MODEL_ACTION_LENGTH);
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
            }

        }
    }
    return false;
}

bool MqttParser::parseIn(ParserModel &input, char *parsedPath, char *payload) {
    strcpy(payload, input.value);
    input.retained = strcmp(input.type, MSG_TYPE_STATUS) == 0 || strcmp(input.type, MSG_TYPE_INIT) == 0;
    if (strcmp(input.type, MSG_TYPE_STATUS) == 0) {
        sprintf(parsedPath, "%s/%s/%d", input.type, input.subject, input.idx);
        return true;
    }
    if (strcmp(input.type, MSG_TYPE_HEALTH) == 0 || strcmp(input.type, MSG_TYPE_OTA) == 0) {
        sprintf(parsedPath, "%s/%s", input.type, input.subject);
        return true;
    }
    sprintf(parsedPath, "%s", input.type);
    return true;
}

MqttParser MQTT_Parser;
