#include <assert.h>
#include "utf8.h"

int utf8_width(unsigned char c) {
    if ((c & 0x80) == 0) {
        return 1;
    } else if ((c & 0xE0) == 0xC0) {
        return 2;
    } else if ((c & 0xF0) == 0xE0) {
        return 3;
    } else if ((c & 0xF8) == 0xF0) {
        return 4;
    }

    assert(0 && "invalid utf8 character encountered");
}