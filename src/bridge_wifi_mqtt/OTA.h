//
// Created by SHL on 11.05.2020.
//

#ifndef OTA_H
#define OTA_H

#include <PubSubClient.h>
#include <Stream.h>
#include <SD.h>


class OTA {

public:

    explicit OTA(WiFiClientSecure &client): _client(client) {
    };

    const char* install(const char * url);
    void apply();

    int connect(char *host);

private:
    WiFiClientSecure _client;

};


#endif //OTA_H
