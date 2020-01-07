//
// Created by SHL on 25.12.2019.
//

#include "persistence.h"
#include <EEPROM.h>

void Persistence::init() {
    EEPROM.begin(size);

}

void Persistence::read(uint8_t idx, T &t) {
    return EEPROM.get(idx, t);
}

void Persistence::write(uint8_t idx, T &t) {
    uint8_t size = sizeof(t);
    EEPROM.put(idx,t);
}

Persistence::Persistence() {
}

