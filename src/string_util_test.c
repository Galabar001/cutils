#include "string_util.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


static void test_string_copy() {
  const char* s = "Hello World!";
  char* s_copy;
  assert(!string_copy(s, &s_copy));
  assert(!strcmp(s, s_copy));
  free(s_copy);
}


int main(int argc, char** argv) {
  test_string_copy();
  return 0;
}
