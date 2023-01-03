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

    const char **process(const char *atCommand);

private:
    Context *ctx;

    void restart();

    const char *getValue(const char *cmd);

    const char *setOrGetCtxValue(char *ctx_val, const char *value, unsigned char length);

    void persist();

    bool setOrGetValue(bool *_val, const bool * new_val);

    const char *processBool(const char *cmd, bool &traget);

    const char **cmdGet(const char *cmd, const char *result);

    const char **cmdSet(const char *cmd, const char *result);

    const char **getInfo(char *result);

    const char **cmdSetOrGet(const char *cmd, const char *val, const char *result);

    unsigned char setOrGetValue(unsigned char *_val, const unsigned char *new_val);

    const char *processInt(const char *cmd, unsigned char &target);

    const char *processLong(const char *cmd, long &target);

    long setOrGetValue(long *_val, const long *new_val);
};
#endif //POWEROID_MODULES_ATCOMMANDS_H
