#include <assert.h>
#include "compound.h"

extern struct array_members am;

int main() {
  assert(am.a[0] == 1);
  assert(am.a[1] == 2);
  assert(am.a[2] == 3);
  assert(am.b == 4);
}
