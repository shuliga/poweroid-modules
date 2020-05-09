//
// Created by SHL on 03.01.2020.
//

#ifndef POWEROID_MODULES_CIRCULARBUFFER_H
#define POWEROID_MODULES_CIRCULARBUFFER_H

#include <Arduino.h>
#include "ParserModel.h"

#define BUFF_SIZE 16

class CircularBuffer {
public:
    void reset();
    bool put(ParserModel &item);
    ParserModel * poll();
    ParserModel * peek();
    uint8_t length();
    bool isEmpty();
    bool isFull();

private:
    uint8_t head;
    uint8_t tail;
    ParserModel buffer[BUFF_SIZE];
};


#endif //POWEROID_MODULES_CIRCULARBUFFER_H
