//
// Created by SHL on 11.12.2020.
//

#include <Arduino.h>

#ifndef POWEROID_MODULES_PORT_H
#define POWEROID_MODULES_PORT_H

typedef uint8_t Ports[3];

#define mask_set(val, i, fill)  (0xFF - (fill << i))
#define pos_set(val, i)  (val << i)

#define PORT_IND PORTS[0]
#define PORT_BUT PORTS[1]
#define PORT_ENC PORTS[2]

#define PORT_IND_1 PORT_IND & 1
#define PORT_IND_2 PORT_IND & 2
#define PORT_IND_3 PORT_IND & 4

#define SET_PORT_IND_1(val) PORT_IND &= mask_set(val, 0, 1UL); PORT_IND |= pos_set(val, 0)
#define SET_PORT_IND_2(val) PORT_IND &= mask_set(val, 1, 1UL); PORT_IND |= pos_set(val, 1)
#define SET_PORT_IND_3(val) PORT_IND &= mask_set(val, 2, 1UL); PORT_IND |= pos_set(val, 2)

#define PORT_BUT_1 PORT_BUT & 1
#define PORT_BUT_2 PORT_BUT & 2
#define PORT_BUT_3 PORT_BUT & 4

#define SET_PORT_BUT_1(val) PORT_BUT &= mask_set(val, 0, 0x3); PORT_BUT |= pos_set(val, 0)
#define SET_PORT_BUT_2(val) PORT_BUT &= mask_set(val, 2, 0x3); PORT_BUT |= pos_set(val, 2)
#define SET_PORT_BUT_3(val) PORT_BUT &= mask_set(val, 4, 0x3); PORT_BUT |= pos_set(val, 4)

#define PORT_ENC_VAL_INC PORT_ENC_VAL & 1
#define PORT_ENC_VAL_DEC PORT_ENC_VAL & 2
#define PORT_ENC_BTN_VAL PORT_ENC_BTN & 4

#define SET_PORT_ENC_VAL(val) PORT_ENC &= mask_set(val, 0, 0x3); PORT_ENC |= pos_set(val, 0)
#define SET_PORT_ENC_BTN(val) PORT_ENC &= mask_set(val, 2, 0x3); PORT_ENC |= pos_set(val, 2)

Ports PORTS;

#endif //POWEROID_MODULES_PORT_H
