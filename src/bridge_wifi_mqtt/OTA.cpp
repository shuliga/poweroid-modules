//
// Created by SHL on 11.05.2020.
//

#include <ESP8266httpUpdate.h>
#include "OTA.h"

static const char* ret_nu = "OTA - no updates";
static const char* ret_ok = "OK";
static char ret_err[64];

int OTA::connect(char * host){
    return _client.connect(host, 443);
}

const char* OTA::install(const char * url){
    ESPhttpUpdate.rebootOnUpdate(false);
    _client.setInsecure();
//    _client.setFingerprint("6A 4B B4 DB B5 F7 5A 40 04 7D BF FE E3 25 43 FF 9B 21 1C 3C");
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
