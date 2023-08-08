#pragma once
#include "cstdio"

#define write_log(format,args...) do { \
        printf(format, ## args); \
    } while(0)
