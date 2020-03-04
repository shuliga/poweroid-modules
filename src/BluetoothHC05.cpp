//
// Created by SHL on 08.01.2020.
//

#include "BluetoothHC05.h"

#define AT_SPEED 38400
#define DEFAULT_TIMEOUT 300

#define HC_05_VER_LEN 2
static const char * HC_05_VER[HC_05_VER_LEN] = {"+VERSION:hc01.comV2.1","VERSION:3.0-20170609"};

bool BluetoothHC05::isHC05inAT() {
    String res = execAtCommand("AT+VERSION");
    for (auto & i : HC_05_VER){
        if (strncmp(res.c_str(), i, strlen(i)) == 0)
            return true;
    }
    return false;
}

bool BluetoothHC05::isInAt(){
    return  isOK(execAtCommand("AT"));
}

bool BluetoothHC05::isInMasterMode(){
    return execAtCommand("AT+ROLE?").startsWith("+ROLE:1");
}

bool BluetoothHC05::programAsSlave() {
    execAtCommand("AT+DISC");
    if (!isOK(execAtCommand("AT+ORGL"))) return false;
    if (!isOK(execAtCommand("AT+RMAAD"))) return false;
    if (!isOK(execAtCommand("AT+NAME=", DEVICE_ID, 0))) return false;
    if (!isOK(execAtCommand("AT+PSWD=", "1234", 0))) return false;
    if (!isOK(execAtCommand("AT+ROLE=", "0", 0))) return false;
    char uart[11];
    sprintf(uart, "%ld,1,0", serial_speed);
    if (!isOK(execAtCommand("AT+UART=", uart, 0))) return false;
    return isOK(execAtCommand("AT+RESET"));

}

bool BluetoothHC05::programAsMaster() {

    if (!isOK(execAtCommand("AT+ORGL"))) return false;
    if (!isOK(execAtCommand("AT+PIO=", "11,1", 0))) return false;
    if (!isOK(execAtCommand("AT+RMAAD"))) return false;

    if (!isOK(execAtCommand("AT+NAME=", DEVICE_ID, 0))) return false;
    if (!isOK(execAtCommand("AT+ROLE=", "1", 0))) return false;
    if (!isOK(execAtCommand("AT+POLAR=", "1,0", 0))) return false;

    execAtCommand("AT+INIT", NULL, 500);

    if (!isOK(execAtCommand("AT+CMODE=", "0", 0))) return false;
    if (!isOK(execAtCommand("AT+CLASS=", "1F00", 0))) return false;
    if (!isOK(execAtCommand("AT+INQM=", "1,2,3", 0))) return false;

    char uart[11];
    sprintf(uart, "%ld,1,0", serial_speed);
    if (!isOK(execAtCommand("AT+UART=", uart, 0))) return false;

    String device = execAtCommand("AT+INQ", NULL, 4000);
    if (device.startsWith("+INQ:")){
        String address =  device.substring(device.indexOf(':') + 1, device.indexOf(","));
        address.replace(':',',');
        if (!isOK(execAtCommand("AT+PSWD=", "1234", 0))) return false;
        if (!isOK(execAtCommand("AT+BIND=", address.c_str(), 0))) return false;

        if (!isOK(execAtCommand("AT+RESET"))) return false;
    }
    return true;
}

bool BluetoothHC05::isOK(const String &res){
    return strncmp(res.c_str(), "OK", 2) == 0;
}

String BluetoothHC05::execAtCommand(const char *cmd) {
    return execAtCommand(cmd, NULL, 0);
}

String BluetoothHC05::execAtCommand(const char *cmd, const char *cmd2, unsigned long timeout) {
    pushCommand(cmd, cmd2, true);

    if(timeout != 0) delay(timeout);
    String res = Serial.readString();

    while(Serial.available()){
        Serial.readString();
    }

    return res;

}

void BluetoothHC05::pushCommand(const char * cmd, const char *cmd2, bool crlf) {
    Serial.print(cmd);
    if (cmd2 != NULL)
        Serial.print(cmd2);
    if (crlf)
        Serial.println();
    Serial.flush();
}

void BluetoothHC05::begin(long _serial_speed) {
    serial_speed = _serial_speed;
    Serial.flush();
    Serial.end();
    Serial.setTimeout(DEFAULT_TIMEOUT);
    Serial.begin(AT_SPEED);
    delay(250);
    Serial.println();
}

void BluetoothHC05::end() {
    Serial.flush();
    Serial.end();
    Serial.setTimeout(150);
    Serial.begin(serial_speed);
}

BluetoothHC05::BluetoothHC05(const char *deviceId) : DEVICE_ID(deviceId) {}