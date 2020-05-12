//
// Created by SHL on 11.05.2020.
//

#include <ESP8266httpUpdate.h>
#include "OTA.h"

bool  OTA::checkSignature(){
    return true;
}

bool  OTA::install(){
    t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.0.2", 80, "/esp/update/arduino.php", "здесь опциональный аргумент с версией");
    return ESP.updateSketch(*in, size, true, false);
}

void  OTA::apply(){
    ESP.restart();
    delay(10000);
}
