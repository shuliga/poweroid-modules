//
// Created by SHL on 03.01.2020.
//

#include "CircularBuffer.h"


void CircularBuffer::reset() {
    tail = 0;
    head = 0;
}

uint8_t CircularBuffer::length() {
    uint8_t size = BUFF_SIZE;
    if (!isFull()) {
        if (head >= tail) {
            size = (head - tail);
        } else {
            size = (BUFF_SIZE + head - tail);
        }
    }
    return size;
}

bool CircularBuffer::isEmpty() {
    return head == tail;
}

bool CircularBuffer::isFull() {
    return (head + 1) % BUFF_SIZE == tail;
}

ParserModel * CircularBuffer::poll() {
    ParserModel * result = NULL;
    if (!isEmpty()) {
        result =  &buffer[tail];
        tail = (tail + 1) % BUFF_SIZE;
    }
    return result;
}

ParserModel * CircularBuffer::peek() {
    ParserModel * result = NULL;
    if (!isEmpty()) {
        result =  &buffer[tail];
    }
    return result;
}

bool CircularBuffer::put(ParserModel &item) {
    bool overflow = false;
    if (isFull()){
        tail = (tail + 1) % BUFF_SIZE;
        overflow = true;
    }
    buffer[head] = item;
    head = (head + 1) % BUFF_SIZE;
    return overflow;
}
