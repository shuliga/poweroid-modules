//
// Created by SHL on 07.01.2020.
//

#include "PoweroidProcessor.h"
#include "PoweroidParser.h"
#include "ParserConstants.h"

PoweroidProcessor::PoweroidProcessor(CircularBuffer *in, CircularBuffer *out, Context *ctx) : IN(in), OUT(out), CTX(ctx) {}

void PoweroidProcessor::processOut(String cmd) {

    ParserModel msg, rawMsg;

    if (!testBannerAndUpdateCnt(cmd) || banner_cnt % BANNER_RATE == 0) {
        outputParsedMessage(cmd, msg);
        outputRawMessage(cmd.c_str(), rawMsg);
    }
}

unsigned char PoweroidProcessor::testBannerAndUpdateCnt(const String &cmd) {
    bool is_banner = cmd.startsWith(SUBJ_PWR_REMOTE_STATE);
    if (is_banner)
        banner_cnt++;
    return is_banner;
}

void PoweroidProcessor::processIn() const {
    if (!this->IN->isEmpty()) {
        ParserModel msg = this->IN->poll();
        Serial.print("Sending IN msg: ");
        Serial.print(msg.type);
        Serial.print("/");
        Serial.println(msg.subject);
        char parsed[64];
        if (strcmp(msg.type, MSG_TYPE_IN) == 0) {
            Serial.println(msg.value);
        } else {
            if (PWR_Parser.parseIn(msg, parsed)) {
                Serial.println(parsed);
            }
        }
    }
}

void PoweroidProcessor::outputParsedMessage(String &cmd, ParserModel &_msg) {
    if (PWR_Parser.parseOut(cmd, _msg)) {
        strcpy(_msg.type, MSG_TYPE_STATUS);
        if (CTX->verbose){
            Serial.print("Parsed CMD: ");
            Serial.println(cmd);
        }
        OUT->put(_msg);
    }
}

void PoweroidProcessor::outputRawMessage(const char *_cmd, ParserModel &_rawMsg) {
    strcpy(_rawMsg.type, MSG_TYPE_OUT);
    _rawMsg.subject[0] = '\0';
    _rawMsg.idx = 0;
    strcpy(_rawMsg.value, _cmd);
    OUT->put(_rawMsg);
}