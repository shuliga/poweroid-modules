//
// Created by SHL on 11.05.2020.
//

#ifndef OTA_H
#define OTA_H

#include <PubSubClient.h>
#include <Stream.h>
#include <SD.h>


class OTA {

    OTA(Stream *_in, const uint32_t _size): size(_size), in(_in) {
    };

public:

    bool checkSignature();
    bool install();
    void apply();

private:
    uint32_t size;
    Stream *in;
};


#endif //OTA_H
