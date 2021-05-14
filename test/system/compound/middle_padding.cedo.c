#include "compound.h"

struct middle_padding two[2] = {
  (struct middle_padding) {.c = 'c', .a = 5},
  (struct middle_padding) {.c = 'd', .a = 0xabcdef12},
};

int main() {}
