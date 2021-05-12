#include <assert.h>

using using_t = int;

extern using_t a;

int main() {
  assert(a == 1234);
}
