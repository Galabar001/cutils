#include "string_util.h"

#include <stdlib.h>
#include <string.h>


error_t string_copy(const char* s, char** result) {
  char* tmp = malloc(strlen(s) + 1);
  if (!tmp) {
    return ERROR_OUT_OF_MEMORY;
  }
  strcpy(tmp, s);
  *result = tmp;
  return 0;
}


