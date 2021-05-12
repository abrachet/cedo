#include <assert.h>
#include "multilevel_typedef.h"

extern indirect_int_t a;

int main() {
  assert(a == 1234567);
}
