//
// Created by SHL on 08.01.2020.
//

#ifndef POWEROID_MODULES_BLUETOOTHHC05_H
#define POWEROID_MODULES_BLUETOOTHHC05_H

#include <Arduino.h>

class BluetoothHC05 {
public:
    explicit BluetoothHC05(const char *deviceId);

    bool isHC05inAT();
    bool programAsSlave();
    bool programAsMaster();

    void begin(long _serial_speed);
    void end();

private:
    const char  * DEVICE_ID;
    long serial_speed = 115200;

    String execAtCommand(const char * cmd, const char *cmd2, unsigned long timeout);

    void pushCommand(const char *cmd, const char *cmd2, bool crlf);

    String execAtCommand(const char *cmd);

    bool isOK(const String &res);

    bool reset();

};

#endif //POWEROID_MODULES_BLUETOOTHHC05_H
