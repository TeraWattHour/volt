#ifndef VOLT_HASHTABLE_H
#define VOLT_HASHTABLE_H
#include <stddef.h>

unsigned long djb2(const unsigned char *str);
unsigned long djb2_between(const unsigned char *start, const unsigned char *end);

#endif //VOLT_HASHTABLE_H
