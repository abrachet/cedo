#include <assert.h>

typedef int int_t;

extern int_t a;

int main() {
  assert(a == 0x12345678);
}
