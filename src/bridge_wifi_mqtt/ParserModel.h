//
// Created by SHL on 01.01.2020.
//

#ifndef POWEROID_MODULES_PARSERMODEL_H
#define POWEROID_MODULES_PARSERMODEL_H

#include <Arduino.h>

/*
 * Sample topic
 * rent/ua-lv-79004-nv-7-4/TERMO_2X2_PWR_1.0-23/cmd/set/prop/1
 *
 */

#define MODEL_VAL_LENGTH 512
#define MODEL_SUBJ_LENGTH 16
#define MODEL_DEVICE_LENGTH 16
#define MODEL_ACTION_LENGTH 8
#define MODEL_MODE_LENGTH 8

struct ParserModel {
    char subject[MODEL_SUBJ_LENGTH + 1] = {'\0'};
    uint8_t idx = 0;
    char value[MODEL_VAL_LENGTH + 1];
    char action[MODEL_ACTION_LENGTH + 1];
    char mode[MODEL_MODE_LENGTH + 1];
    char device[MODEL_DEVICE_LENGTH + 1] = {'\0'};
    bool retained;
};

#endif //POWEROID_MODULES_PARSERMODEL_H
