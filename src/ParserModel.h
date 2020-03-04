//
// Created by SHL on 01.01.2020.
//

#ifndef POWEROID_MODULES_PARSERMODEL_H
#define POWEROID_MODULES_PARSERMODEL_H

#include <Arduino.h>

#define MODEL_VAL_LENGTH 128
#define MODEL_SUBJ_LENGTH 16
#define MODEL_DEVICE_LENGTH 16
#define MODEL_MODE_LENGTH 8
#define MODEL_TYPE_LENGTH 8

struct ParserModel {
    char subject[MODEL_SUBJ_LENGTH + 1];
    uint8_t idx;
    char value[MODEL_VAL_LENGTH + 1];
    char mode[MODEL_MODE_LENGTH + 1];
    char type[MODEL_TYPE_LENGTH + 1];
    char device[MODEL_DEVICE_LENGTH + 1] = {'\0'};
};

#endif //POWEROID_MODULES_PARSERMODEL_H
