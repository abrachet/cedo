#include <assert.h>

extern int array[5];

int main() {
  for (int i = 0; i < 5; i++)
    assert(array[i] == i);
}
