//
// Created by SHL on 08.01.2020.
//

#include "BluetoothHC05.h"

#define AT_COMMAND(C) if (!isOK(execAtCommand("AT+(C)"))) return false;
#define AT_COMMAND2(C1,C2) if (!isOK(execAtCommand("AT+(C1)", (C2), 0 ))) return false;
#define AT_RESPONSE(C,T) execAtCommand("AT+(C1)", NULL, (T));

#define AT_SPEED 38400
#define DEFAULT_TIMEOUT 250

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

bool BluetoothHC05::programAsSlave() {
    AT_COMMAND(DISC)
    AT_COMMAND(ORGL)
    AT_COMMAND(RMAAD)
    AT_COMMAND2(NAME=,"PWM-BMU-01")
    AT_COMMAND2(PSWD=,"1234")
    AT_COMMAND2(ROLE=,"0")
    AT_COMMAND(RESET)

    return true;
}

bool BluetoothHC05::programAsMaster() {
    AT_COMMAND(ORGL)
    AT_COMMAND2(PIO=,"11,1")
    AT_COMMAND(RMAAD)
    AT_COMMAND2(NAME=, DEVICE_ID)
    AT_COMMAND2(ROLE=,"1")
    AT_COMMAND2(POLAR=,"1,0")

    reset();

    AT_COMMAND2(CMODE=,"0")
    AT_COMMAND2(CLASS=,"1F00")
    AT_COMMAND2(INQM=,"1,2,3")

    char uart[11];
    sprintf(uart, "%ld,1,0", serial_speed);
    AT_COMMAND2(UART=,uart)

    String device = AT_RESPONSE(+INQM,4000)
    if (device.startsWith("+INQ:")){
        String address =  device.substring(device.indexOf(':') + 1, device.indexOf(","));
        address.replace(':',',');
        AT_COMMAND2(BIND=,address.c_str())

        AT_COMMAND2(PSWD=,"1234")
        AT_COMMAND2(POLAR=,"1,1")
        reset();
        AT_COMMAND2(POLAR=,"1,0")
    }

    return true;
}

bool BluetoothHC05::reset(){
    AT_COMMAND(RESET)
    delay(1000);
    AT_COMMAND(INIT)
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

    Serial.setTimeout(timeout == 0 ? DEFAULT_TIMEOUT : timeout );

    String res = Serial.readString();
    while(Serial.available()){
        Serial.readString();
    }

    Serial.setTimeout(DEFAULT_TIMEOUT);
    return res;

}

void BluetoothHC05::pushCommand(const char * cmd, const char *cmd2, bool crlf) {
    Serial.print(cmd);
    if (cmd2 != NULL)
        Serial.print(cmd2);
    if (crlf)
        Serial.println();
}

void BluetoothHC05::begin(long _serial_speed) {
    serial_speed = _serial_speed;
    Serial.flush();
    Serial.end();
    Serial.begin(AT_SPEED);
}

void BluetoothHC05::end() {
    Serial.flush();
    Serial.end();
    Serial.begin(serial_speed);
}

BluetoothHC05::BluetoothHC05(const char *deviceId) : DEVICE_ID(deviceId) {}