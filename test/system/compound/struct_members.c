#include <assert.h>
#include <string.h>
#include "compound.h"

struct struct_members expected = {
  .a = {
    .a = 1,
    .b = 2,
    .c = 3,
    .d = 4
  },
  .b = {
    .a = 5,
    .b = 6,
    .c = 7,
    .d = 8
  }
};

extern struct struct_members sm;

int main() {
  assert(!memcmp(&expected, &sm, sizeof(struct struct_members)));
}
