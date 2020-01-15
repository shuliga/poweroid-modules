//
// Created by SHL on 09.01.2020.
//

#ifndef POWEROID_MODULES_TIMECLIENT_H
#define POWEROID_MODULES_TIMECLIENT_H

#include <NtpClientLib.h>

class TimeClient {
public:
    TimeClient();

    void begin();
    bool isReady();
    void end();
    void getTimestamp(char *timestamp);
};

extern TimeClient TIME;

#endif //POWEROID_MODULES_TIMECLIENT_H
