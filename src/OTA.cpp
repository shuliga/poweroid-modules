//
// Created by SHL on 11.05.2020.
//

#include <ESP8266httpUpdate.h>
#include "OTA.h"

static const char* ret_nu = "OTA - no updates";
static const char* ret_ok = "OK";
static char ret_err[64];

const char* OTA::install(const char * url){
    ESPhttpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ret = ESPhttpUpdate.update(_client, url);
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            sprintf(ret_err, "OTA Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            return ret_err;
        case HTTP_UPDATE_NO_UPDATES:
            return ret_nu;
        case HTTP_UPDATE_OK:
            return ret_ok;
    }
    return NULL;
}

void  OTA::apply(){
    ESP.restart();
    delay(10000);
}
