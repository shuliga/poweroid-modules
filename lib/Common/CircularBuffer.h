//
// Created by SHL on 03.01.2020.
//

#ifndef POWEROID_MODULES_CIRCULARBUFFER_H
#define POWEROID_MODULES_CIRCULARBUFFER_H

#include <Arduino.h>

#define CIRCULAR_BUFF_SIZE 16

template <class T>
class CircularBuffer {
public:
    void reset();
    bool put(T&);
    T * poll();
    T * peek();
    uint8_t length();
    bool isEmpty();
    bool isFull();

private:
    uint8_t head;
    uint8_t tail;
    T buffer[CIRCULAR_BUFF_SIZE];
};

template <class T>
void CircularBuffer<T>::reset() {
    tail = 0;
    head = 0;
}

template <class T>
uint8_t CircularBuffer<T>::length() {
    uint8_t size = CIRCULAR_BUFF_SIZE;
    if (!isFull()) {
        if (head >= tail) {
            size = (head - tail);
        } else {
            size = (CIRCULAR_BUFF_SIZE + head - tail);
        }
    }
    return size;
}

template <class T>
bool CircularBuffer<T>::isEmpty() {
    return head == tail;
}

template <class T>
bool CircularBuffer<T>::isFull() {
    return (head + 1) % CIRCULAR_BUFF_SIZE == tail;
}

template <class T>
T * CircularBuffer<T>::poll() {
    T * result = NULL;
    if (!isEmpty()) {
        result =  &buffer[tail];
        tail = (tail + 1) % CIRCULAR_BUFF_SIZE;
    }
    return result;
}

template <class T>
T * CircularBuffer<T>::peek() {
    T * result = NULL;
    if (!isEmpty()) {
        result =  &buffer[tail];
    }
    return result;
}

template <class T>
bool CircularBuffer<T>::put(T& item) {
    bool overflow = false;
    if (isFull()){
        tail = (tail + 1) % CIRCULAR_BUFF_SIZE;
        overflow = true;
    }
    buffer[head] = item;
    head = (head + 1) % CIRCULAR_BUFF_SIZE;
    return overflow;
}

#endif //POWEROID_MODULES_CIRCULARBUFFER_H
