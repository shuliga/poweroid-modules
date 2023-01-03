//
// Created by SHL on 12.05.2020.
//

#ifndef POWEROID_MODULES_GLOBAL_H
#define POWEROID_MODULES_GLOBAL_H

#define GLOBAL_LEN_TOPIC 96

struct Cmd {
    bool subscribe = false;
    bool initiate = false;
    bool wifiConnect = false;
    bool uartConnect = false;
    bool mqttConnect = false;
    bool updateToken = false;
};

struct Status {
    bool uartConnected = false;
    bool wifiConnected = false;
    bool mqttConnected = false;
    bool mqttDisconnectLatch = false;
    bool initiated = false;
    bool tokenResponded = false;
    unsigned long counter_2Hz = 0;
    bool timer_5s = false;
    bool timer_2_5s = false;
    bool timer_0_5s = false;
};

struct Topics {
    char prefix[GLOBAL_LEN_TOPIC + 1];
    char pub_topic[GLOBAL_LEN_TOPIC + 1];
    char sub_uart_device_topic[GLOBAL_LEN_TOPIC + 1];
    char sub_module_topic[GLOBAL_LEN_TOPIC + 1];
};

struct Flag {
    bool connect = true;
    bool mqttConnect = true;
    bool wifiTryConnect = true;
    bool verbose = false;
    bool master = true;
};

struct Global {
    Cmd cmd;
    Flag flag;
    Status status;
    Topics topics;
};


extern Global GLOBAL;



#endif //POWEROID_MODULES_GLOBAL_H
