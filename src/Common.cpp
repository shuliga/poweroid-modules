//
// Created by SHL on 07.01.2020.
//

#include "Common.h"

Global GLOBAL;

void printToSerial(const char * c1){
    if (GLOBAL.verbose)
        Serial.println(c1);
}

void printToSerial(const char *c1, const char *c2) {
    if (GLOBAL.verbose){
        Serial.print(c1);
        Serial.println(c2);
    }
}

void printToSerial(const char *c1, const char *c2, const char *c3) {
    if (GLOBAL.verbose){
        Serial.print(c1);
        Serial.print(c2);
        Serial.println(c3);
    }
}

void printToSerial(const char *c1, const char *c2, const char *c3, const char *c4) {
    if (GLOBAL.verbose){
        Serial.print(c1);
        Serial.print(c2);
        Serial.print(c3);
        Serial.println(c4);
    }
}

bool strEndsWith(const char * str, const char * suffix) {

    if( str == NULL || suffix == NULL )
        return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len)
        return 0;

    return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

bool startsWith(const char * _cmd, const char * pref){
    return strncmp(pref, _cmd, strlen(pref)) == 0;
}




