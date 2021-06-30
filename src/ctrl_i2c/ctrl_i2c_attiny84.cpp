//
// Created by SHL on 07.12.2020.
//

const char *DEVICE_ID = "PWM-BMU-01"; // Poweroid module, Controller I2C
char DEVICE_ADDR = 0x25;

#define I2C_SLAVE_ADDR 0x25

#include <Arduino.h>
#include "WireS.h"
#include "Encoder.h"
#include "MultiClick.h"
#include "CircularBuffer.h"

#include "ports.h"
#include "pins.h"

#define E_NOE 0
#define E_CLICK 1
#define E_DBL_CLICK 2
#define E_HOLD 3
#define E_INC 2
#define E_DEC 1

int32_t encoder_pos = 0;

CircularBuffer<uint8_t> buttBuff;
CircularBuffer<uint8_t> encBuff;

MultiClick encoderClick = MultiClick(ENC_BTN_PIN);
MultiClick encoderInc = MultiClick(ENC_INC_PIN);
MultiClick encoderDec = MultiClick(ENC_DEC_PIN);

MultiClick btn1click = MultiClick(BTN_1_PIN);
MultiClick btn2click = MultiClick(BTN_2_PIN);
MultiClick btn3click = MultiClick(BTN_3_PIN);
//Encoder encoder = Encoder(ENC_INC_PIN, ENC_DEC_PIN);

void log();

void requestEvent() {
    if (!encBuff.isEmpty()) {
        TinyWireS.write(*encBuff.poll());
    } else {
        TinyWireS.write(E_NOE);
    }
    if (!buttBuff.isEmpty()) {
        TinyWireS.write(*buttBuff.poll());
    } else {
        TinyWireS.write(E_NOE);
    }
}

void lightIndicators(){
    digitalWrite(IND_1_PIN, PORT_IND_1 ? HIGH : LOW);
    digitalWrite(IND_2_PIN, PORT_IND_2 ? HIGH : LOW);
    digitalWrite(IND_3_PIN, PORT_IND_3 ? HIGH : LOW);
}

void receiveEvent(size_t size) {
    if (size >= 1) {
        while (TinyWireS.available()){
            PORT_IND = TinyWireS.readByte();
        }
    }
}

uint8_t mapButton(McEvent enc){
    switch (enc) {
        case CLICK: return E_CLICK;
        case DOUBLE_CLICK: return E_DBL_CLICK;
        case HOLD: return E_HOLD;
        default: return E_NOE;
    }
}

uint8_t mapEnc(McEvent inc, McEvent dec){
    if (inc == CLICK) {
        return E_INC;
    } else if (dec == CLICK) {
        return E_DEC;
    }
    return E_NOE;
}

void setup(){
    TinyWireS.begin(I2C_SLAVE_ADDR);
    TinyWireS.onRequest(requestEvent);
    TinyWireS.onReceive(receiveEvent);

    pinMode(IND_1_PIN, OUTPUT);
    digitalWrite(IND_1_PIN, LOW);
    pinMode(IND_2_PIN, OUTPUT);
    digitalWrite(IND_2_PIN, LOW);
    pinMode(IND_3_PIN, OUTPUT);
    digitalWrite(IND_3_PIN, LOW);

//    Serial1.begin(115200);
//    Serial1.write("ATTiny started\n");

}

void loop(){

    SET_PORT_ENC_BTN(mapButton(encoderClick.checkButton()));

    uint8_t ce1 = mapButton(btn1click.checkButton());
    uint8_t ce2 = mapButton(btn2click.checkButton());
    uint8_t ce3 = mapButton(btn3click.checkButton());

    SET_PORT_BUT_1(ce1);
    SET_PORT_BUT_2(ce2);
    SET_PORT_BUT_3(ce3);

    if (ce1 != E_NOE || ce2 != E_NOE || ce3 != E_NOE) {
        buttBuff.put(PORT_BUT);
    }

    uint8_t ee1 = mapButton(encoderClick.checkButton());
    uint8_t ee2 = mapEnc(encoderInc.checkButton(), encoderDec.checkButton());

    SET_PORT_ENC_BTN(ee1);
    SET_PORT_ENC_VAL(ee2);


    if (ee1 != E_NOE || ee2 != E_NOE) {
        encBuff.put(PORT_ENC);
    }

//    uint32_t encCurr = encoder.readAndReset();
//    uint8_t delta = encoder_pos - encCurr;
//    for (uint8_t i; i < abs(delta); i++){
//        if (delta > 0) {
//            SET_PORT_ENC_VAL(E_DEC);
//        } else if (delta < 0) {
//            SET_PORT_ENC_VAL(E_INC);
//        }
//        encBuff.put(PORT_ENC_VAL);
//    }
//
//    encoder_pos = encCurr;


    lightIndicators();

//    log();
}

void log() {

    char binarr[17];

    if (!buttBuff.isEmpty()){
        buttBuff.poll();
        Serial1.write("B:");
        itoa(PORT_BUT, binarr,2);
        Serial1.write(binarr);
        Serial1.write('\n');
    }

    if (!encBuff.isEmpty()) {
        encBuff.poll();
        Serial1.write("E:");
        itoa(PORT_ENC, binarr, 2);
        Serial1.write(binarr);
        Serial1.write('\n');
    }

}
