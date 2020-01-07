//
// Created by SHL on 01.01.2020.
//

#ifndef POWEROID_MODULES_POWEROIDPARSER_H
#define POWEROID_MODULES_POWEROIDPARSER_H

#include "ParserModel.h"

class PoweroidParser {
public:
    bool parseOut(const String &input, ParserModel &parsed);
    bool parseIn(ParserModel &input, char *parsed);

private:
    uint8_t extractFromBrackets(const String &input) const;

    String extractValue(const String &input) const;

    int8_t nthIndexOf(const char *str, const char chr, uint8_t nth);
};

extern PoweroidParser PWR_Parser;

#endif //POWEROID_MODULES_POWEROIDPARSER_H
