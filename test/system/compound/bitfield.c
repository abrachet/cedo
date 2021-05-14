#include <assert.h>
#include "compound.h"

extern struct bitfield bitfield;

int main() {
  assert(bitfield.a == 3);
  assert(bitfield.b == 4);
  assert(bitfield.c == 91);
}
