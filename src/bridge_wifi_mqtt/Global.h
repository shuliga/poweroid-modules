//
// Created by SHL on 12.05.2020.
//

#ifndef POWEROID_MODULES_GLOBAL_H
#define POWEROID_MODULES_GLOBAL_H

#define GLOBAL_LEN_TOPIC 96

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
    bool mqttDisconnectLatch = false;
    bool initiated = false;
    unsigned long counter_2Hz = 0;
    bool timer_5s = false;
    bool timer_2_5s = false;
    bool timer_0_5s = false;
};

struct Topics {
    char prefix[GLOBAL_LEN_TOPIC + 1];
    char pub_topic[GLOBAL_LEN_TOPIC + 1];
    char sub_uart_topic[GLOBAL_LEN_TOPIC + 1];
    char sub_device_topic[GLOBAL_LEN_TOPIC + 1];
};

struct Flag {
    bool connect = true;
    bool mqttConnect = true;
    bool wifiTryConnect = true;
    bool verbose = false;
    bool master = true;
    bool tokenMode = false;
    uint8_t tokenId = 0;
};

struct Global {
    Cmd cmd;
    Flag flag;
    Status status;
    Topics topics;
};


extern Global GLOBAL;



#endif //POWEROID_MODULES_GLOBAL_H
