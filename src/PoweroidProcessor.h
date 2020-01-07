//
// Created by SHL on 07.01.2020.
//

#ifndef POWEROID_MODULES_POWEROIDPROCESSOR_H
#define POWEROID_MODULES_POWEROIDPROCESSOR_H

#include <Arduino.h>
#include "CircularBuffer.h"
#include "Context.h"

#define BANNER_RATE 10

class PoweroidProcessor {
private:
    CircularBuffer *IN, *OUT;
    Context *CTX;
    uint8_t banner_cnt;

    void outputRawMessage(const char *_cmd, ParserModel &rawMsg);
    void outputParsedMessage(String &cmd, ParserModel &msg);
    unsigned char testBannerAndUpdateCnt(const String &cmd);
public:
    PoweroidProcessor(CircularBuffer *in, CircularBuffer *out, Context *ctx);
    void processOut(String cmd);
    void processIn() const;
};


#endif //POWEROID_MODULES_POWEROIDPROCESSOR_H
