//
// Created by SHL on 25.12.2019.
//

#ifndef POWEROID_MODULES_PERSISTENCE_H
#define POWEROID_MODULES_PERSISTENCE_H

#include <Arduino.h>

class Persistence {
public:
    Persistence();
    T read(uint8_t idx);
    void write(uint8_t idx, T &t);

private:
    void init();
};

#endif //POWEROID_MODULES_PERSISTENCE_H
