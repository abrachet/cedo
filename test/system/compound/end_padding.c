#include <assert.h>
#include "compound.h"

extern struct end_padding two[2];

int main() {
  assert(two[0].a == 1);
  assert(two[0].c == 'c');

  assert(two[1].a == 2);
  assert(two[1].c == 'a');
}
