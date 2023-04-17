#include "include/conv.h"
#include "include/bool.h"

#include <string.h>
#include <math.h>

int stoi(char *str) {
    int rslt = 0;

    for (unsigned int i = 0; i < strlen(str); i ++) {
        int old = rslt;
        rslt = rslt * 10 + ctoi(str[i]);

        // Check for arithmetic overflow
        if (rslt < old) {
            // TODO Procrastination my friend
        }
    }

    return rslt;
}

int ctoi(char c) {
    return c - '0';
}

float stof(char *str) {
    float rslt = 0.0f;

    bool region = false; // false - Integer, true - Real
    float real_ix = 1.0f;

    for (unsigned int i = 0; i < strlen(str); i ++) {
        char c = str[i];
        int val = (c == '.') ? -1 : ctoi(c);

        if (c == '.') {
            region = true;
            continue;
        }

        // Integer region
        if (!region) {
            rslt = rslt * 10 + val;
            continue;
        }

        // Real region
        rslt = rslt + (powf(10.0f, -real_ix) * (float) val);
        real_ix ++;
    }

    return rslt;
}