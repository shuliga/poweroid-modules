//
// Created by SHL on 07.01.2020.
//

#ifndef POWEROID_MODULES_COMMON_H
#define POWEROID_MODULES_COMMON_H

#include <Arduino.h>

void logToSerial(const char * c1);
void logToSerial(const char * c1, const char * c2);
void logToSerial(const char *c1, int i);
void logToSerial(const char * c1, const char * c2, const char * c3);
void logToSerial(const char * c1, const char * c2, const char * c3, const char * c4);
bool startsWith(const char * _cmd, const char * pref);
bool strEndsWith(const char * str, const char * suffix);
char *getItem(char *split, uint8_t size, uint8_t idx);
char *getItemBackwards(char *split, uint8_t size, uint8_t idx);
uint8_t splitStr(char *split, const char *src, const char divider);
uint8_t splitLines(char *lines);

#endif //POWEROID_MODULES_COMMON_H
