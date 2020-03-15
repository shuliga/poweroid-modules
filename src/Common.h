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
    bool timer_5s = false;
};

struct Flag {
    bool connect = true;
    bool verbose = true;
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

#endif //POWEROID_MODULES_COMMON_H
