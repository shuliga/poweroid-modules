//
// Created by SHL on 01.01.2020.
//

#include "ParserConstants.h"
#include "PoweroidParser.h"

static const char * cmd_divider = " -> ";
static const char * SETTER_PATTERN = "%s_%s_%d:%s";
static const char * GETTER_PATTERN = "%s_%s_%d";

bool PoweroidParser::parseOut(const String &input, ParserModel &parsed) {
    int8_t divider_start = input.indexOf(cmd_divider);
    if (divider_start > 0) {
        if (input.indexOf(SUBJ_PWR_VERSION) >= 0) {
            strncpy(parsed.subject, SUBJ_MQTT_VER, MODEL_SUBJ_LENGTH);
            strncpy(parsed.value,input.substring(divider_start + strlen(cmd_divider)).c_str(), MODEL_VAL_LENGTH);
            return true;
        }
        if (input.indexOf(SUBJ_PROP) >= 0) {
            parsed.idx = extractFromBrackets(input);
            strncpy(parsed.subject, SUBJ_PROP, MODEL_SUBJ_LENGTH);
            strncpy(parsed.value, extractValue(input).c_str(), MODEL_VAL_LENGTH);
            return true;
        }
        if (input.indexOf(SUBJ_STATE) >= 0) {
            parsed.idx = extractFromBrackets(input);
            strncpy(parsed.subject, SUBJ_STATE, MODEL_SUBJ_LENGTH);
            strncpy(parsed.value, extractValue(input).c_str(), MODEL_VAL_LENGTH);
            return true;
        }
    } else {
        if (input.indexOf('[') >= 0) {
            parsed.idx = extractFromBrackets(input);
            strncpy(parsed.subject, SUBJ_STATE, MODEL_SUBJ_LENGTH);
            strncpy(parsed.value, extractValue(input).c_str(), MODEL_VAL_LENGTH);
            return true;
        }
        if (input.startsWith(SUBJ_PWR_REMOTE_STATE)) {
            uint8_t banner_comma = nthIndexOf(input.c_str(), ',', 4);
            if(banner_comma > 3){
                parsed.idx = input.substring(banner_comma - 1, banner_comma).toInt();
                strncpy(parsed.subject, SUBJ_MQTT_BANNER, MODEL_SUBJ_LENGTH);
                strncpy(parsed.value, input.substring(banner_comma + 1).c_str(), MODEL_VAL_LENGTH);
                return true;
            }
        }
    }
    return false;
}

bool PoweroidParser::parseIn(ParserModel &input, char * parsed) {
    if (strcmp(input.subject, SUBJ_PROP) == 0){
        if(strcmp(input.action, ACTION_SET) == 0){
            sprintf(parsed, SETTER_PATTERN, input.action, input.subject, input.idx, input.value);
            return true;
        }
        if(strcmp(input.action, ACTION_GET) == 0){
            sprintf(parsed, GETTER_PATTERN, input.action, input.subject, input.idx);
            return true;
        }
    }
    if (strcmp(input.subject, SUBJ_STATE) == 0){
        if(strcmp(input.action, ACTION_DISARM) == 0){
            sprintf(parsed, SETTER_PATTERN, input.action, input.subject, input.idx, input.value);
            return true;
        }
        if(strcmp(input.action, ACTION_GET) == 0){
            sprintf(parsed, GETTER_PATTERN, input.action, input.subject, input.idx);
            return true;
        }
    }
    if (strcmp(input.subject, SUBJ_MQTT_VER) == 0){
        sprintf(parsed, "%s_%s", input.action, SUBJ_PWR_VERSION);
        return true;
    }
    return false;
}

uint8_t PoweroidParser::extractFromBrackets(const String &input) const {
    int8_t bracket_open = input.lastIndexOf('[');
    int8_t bracket_close = input.lastIndexOf(']');
    return input.substring(bracket_open + 1, bracket_close).toInt();
}

String PoweroidParser::extractValue(const String &input) const { return input.substring(input.lastIndexOf(':') + 1); }

int8_t PoweroidParser::nthIndexOf(const char * str, const char chr, uint8_t nth){
    uint8_t cnt = 0;
    for(uint8_t i = 0; i < strlen(str); i++){
        if(str[i] == chr)
            cnt++;
        if(cnt == nth){
            return i;
        }
    }
    return -1;
}

PoweroidParser PWR_Parser;
