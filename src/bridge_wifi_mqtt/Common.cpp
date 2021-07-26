//
// Created by SHL on 07.01.2020.
//

#include "Common.h"
#include "Global.h"

Global GLOBAL;

void logToSerial(const char * c1){
    if (GLOBAL.flag.verbose)
        Serial.println(c1);
}

void logToSerial(const char *c1, const char *c2) {
    if (GLOBAL.flag.verbose){
        Serial.printf("%s%s\r\n", c1, c2);
    }
}

void logToSerial(const char *c1, int i) {
    if (GLOBAL.flag.verbose){
        Serial.printf("%s %d\r\n", c1, i);
    }
}

void logToSerial(const char *c1, const char *c2, const char *c3) {
    if (GLOBAL.flag.verbose){
        Serial.printf("%s%s%s\r\n", c1, c2, c3);
    }
}

void logToSerial(const char *c1, const char *c2, const char *c3, const char *c4) {
    if (GLOBAL.flag.verbose){
        Serial.printf("%s%s%s%s\r\n", c1, c2, c3, c4);
    }
}

uint8_t splitStr(char *split, const char *src, const char divider) {
    uint8_t z = 1;
    strcpy(split, src);
    for (uint8_t i = 0; i < strlen(src); i++) {
        if (split[i] == divider) {
            split[i] = '\0';
            z++;
        }
    }
    return z;
}

uint8_t splitLines(char *lines) {
    uint8_t n = 1;
    uint8_t z = 0;
    size_t len = strlen(lines);
    for (size_t i = 0; i < len; i++) {
        if (lines[i] == '\r' || lines[i] == '\n') {
            lines[i] = '\0';
            z++;
        } else {
            if (z != 0) {
                n++;
            }
            z = 0;
        }
    }
    return n;
}

char *getItem(char *split, uint8_t size, uint8_t idx) {
    uint8_t z = 0;
    for (uint8_t i = 0; i < size; i++) {
        if (split[i] == '\0' || i == 0) {
            z++;
        }
        if (z == idx) {
            return &split[i] + (i == 0 ? 0 : 1);
        }
    }
    return NULL;
}

char *getItemBackwards(char *split, uint8_t size, uint8_t idx) {
    uint8_t z = 0;
    for (uint8_t i = size - 1; i >= 0; i--) {
        if (split[i] == '\0' || i == 0) {
            z++;
        }
        if (z == idx) {
            return &split[i] + (i == 0 ? 0 : 1);
        }
    }
    return NULL;
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