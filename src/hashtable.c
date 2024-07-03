#include "hashtable.h"

unsigned long djb2(const unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *(str++)))
        hash = 33 * hash + c;

    return hash;
}

unsigned long djb2_between(const unsigned char *start, const unsigned char *end) {
    unsigned long hash = 5381;
    int c;

    while ((c = *(start++)) && start != end)
        hash = 33 * hash + c;

    return hash;
}