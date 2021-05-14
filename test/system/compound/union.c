#include <assert.h>
#include "compound.h"

extern union basic_union u;

int main() {
  assert(u.i == 12345678);
}
