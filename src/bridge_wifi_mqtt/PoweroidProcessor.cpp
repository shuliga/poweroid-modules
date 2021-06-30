//
// Created by SHL on 07.01.2020.
//

#include "PoweroidProcessor.h"
#include "PoweroidParser.h"
#include "ParserConstants.h"
#include "Common.h"

PoweroidProcessor::PoweroidProcessor(CircularBuffer<ParserModel> *in, CircularBuffer<ParserModel> *out, Context *ctx) : IN(in), OUT(out), CTX(ctx) {}

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
        ParserModel *msg = this->IN->poll();
        bool isRaw = strcmp(msg->type, MSG_TYPE_RAW_IN) == 0;
        logToSerial("PWR - sending IN msg: ", msg->type, "/", isRaw ? msg->value : msg->subject);
        char parsed[MODEL_VAL_LENGTH];
        if (isRaw) {
            Serial.println(msg->value);
        } else {
            if (PWR_Parser.parseIn(*msg, parsed)) {
                Serial.println(parsed);
            }
        }
    }
}

void PoweroidProcessor::outputParsedMessage(String &cmd, ParserModel &_msg) {
    if (PWR_Parser.parseOut(cmd, _msg)) {
        strcpy(_msg.type, MSG_TYPE_STATUS);
        logToSerial("PWR - parsed CMD: ", cmd.c_str());
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