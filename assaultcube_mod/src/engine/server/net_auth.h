#ifndef NETAUTH_H
#define NETAUTH_H
#include "pch.h"
#include "tools/cube_string.h"

struct authkey // for AUTH
{
    char *name, *key, *desc;
    int lastauth;

    authkey(const char *name, const char *key, const char *desc)
        : name(newstring(name)), key(newstring(key)), desc(newstring(desc)),
          lastauth(0)
    {
    }

    ~authkey()
    {
        if (name) {
            delete[] name;
            name = 0;
        }
        if (key) {
            delete[] key;
            key = 0;
        }
        if (desc) {
            delete[] desc;
            desc = 0;
        }
    }
};

#endif