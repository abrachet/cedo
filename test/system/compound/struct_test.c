#include <assert.h>
#include "compound.h"

extern struct basic a;

int main() {
  assert(a.a == 1);
  assert(a.b == 2);
  assert(a.c == 3);
  assert(a.d == 4);
}
