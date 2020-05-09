//
// Created by SHL on 07.01.2020.
//

#ifndef POWEROID_MODULES_COMMON_H
#define POWEROID_MODULES_COMMON_H

#include <Arduino.h>

struct Cmd {
    bool subscribe = false;
    bool initiate = false;
    bool wifiConnected = false;
    bool uartConnected = false;
    bool mqttConnected = false;
};

struct Status {
    bool uartConnected = false;
    bool wifiConnected = false;
    bool mqttConnected = false;
    bool initiated = false;
    unsigned long timer_2Hz = 0;
    bool timer_5s = false;
    bool timer_2_5s = false;
};

struct Flag {
    bool connect = true;
    bool mqttConnect = true;
    bool wifiTryConnect = true;
    bool verbose = false;
    bool master = true;
};

struct Global {
    Cmd cmd;
    Flag flag;
    Status status;
};


extern Global GLOBAL;

void printToSerial(const char * c1);
void printToSerial(const char * c1, const char * c2);
void printToSerial(const char *c1, int i);
void printToSerial(const char * c1, const char * c2, const char * c3);
void printToSerial(const char * c1, const char * c2, const char * c3, const char * c4);
bool startsWith(const char * _cmd, const char * pref);
bool strEndsWith(const char * str, const char * suffix);
char *getItemBackwards(char *split, uint8_t size, uint8_t idx);
uint8_t splitStr(char *split, const char *src, const char divider);
uint8_t splitLines(char *lines);

#endif //POWEROID_MODULES_COMMON_H
