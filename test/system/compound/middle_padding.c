#include <assert.h>
#include "compound.h"

extern struct middle_padding two[2];

int main() {
  assert(two[0].c == 'c');
  assert(two[0].a == 5);

  assert(two[1].c == 'd');
  assert(two[1].a == 0xabcdef12);
}
