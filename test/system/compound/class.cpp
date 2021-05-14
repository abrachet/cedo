#include <cassert>
#include "compound.h"

extern Class c;

int main() {
  assert(c.a == 1);
  assert(c.b == 2);
}
