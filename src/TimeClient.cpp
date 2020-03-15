//
// Created by SHL on 09.01.2020.
//

#include "TimeClient.h"

#define NTP_TIMEOUT 1500

int8_t timeZone = 2;
int8_t minutesTimeZone = 0;

const char *ntpServer = "pool.ntp.org";
static bool ready;

TimeClient::TimeClient() {
    NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
        if(event == timeSyncd)
            ready = true;
    });
}

void TimeClient::begin() {
    ready = false;
    NTP.setInterval(63);
    NTP.setNTPTimeout(NTP_TIMEOUT);
    NTP.begin(ntpServer, timeZone, true, minutesTimeZone);
}

bool TimeClient::isReady(){
    return ready;
}

void TimeClient::getTimestamp(char * timestamp) {
    strcpy(timestamp, NTP.getTimeDateString().c_str());
}

void TimeClient::end() {
    NTP.stop();
    ready = false;
}

TimeClient TIME;