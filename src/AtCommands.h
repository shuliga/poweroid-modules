//
// Created by SHL on 04.01.2020.
//

#ifndef POWEROID_MODULES_ATCOMMANDS_H
#define POWEROID_MODULES_ATCOMMANDS_H

#include "Context.h"

#define INFO "INFO"

class AtCommands {
public:
    explicit AtCommands(Context *ctx);
    const char * process(const char * atCommand);

private:
    Context * ctx;

    void restart();

    const char *getValue(const char *cmd);

    const char *setOrGetCtxValue(char *ctx_val, const char *value, unsigned char length);

    void persist();

    bool setOrGetValue(bool *_val, const bool * new_val);
};

#endif //POWEROID_MODULES_ATCOMMANDS_H
