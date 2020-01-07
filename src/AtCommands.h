//
// Created by SHL on 04.01.2020.
//

#ifndef POWEROID_MODULES_ATCOMMANDS_H
#define POWEROID_MODULES_ATCOMMANDS_H

#include "Context.h"

#define INFO "INFO"

class AtCommands {
public:
    AtCommands(Context *ctx);
    const char * process(const char * atCommand);
    bool strEndsWith(const char *str, const char *suffix);

private:
    Context * ctx;

    void restart();

    const char *getValue(const char *cmd);

    bool startsWith(const char *_cmd, const char *pref);

    const char *setOrGetCtxValue(char *ctx_val, const char *value, unsigned char length);

    void persist();
};

#endif //POWEROID_MODULES_ATCOMMANDS_H
