#include "hash.h"


uint64_t hash_string(const char* s) {
  // TODO: Use better hash here.
  uint64_t hash = 0;
  while (*s) {
    char c = *s++;
    hash += c;
    hash *= c;
    hash += hash >> 8;
    hash += hash >> 16;
    hash += hash >> 32;    
  }
  return hash;
}
