#include "hash.h"

#include <assert.h>


static void test_hash_string() {
  assert(hash_string("abc123") == hash_string("abc123"));
  assert(hash_string("abc123") != hash_string("abc124"));
  assert(hash_string("abc123") != hash_string("bbc123"));  
  assert(hash_string("abaaaa") != hash_string("aabaaa"));  
  assert(hash_string("aaaaaa") != hash_string("aaaaab"));
  assert(hash_string("ba") != hash_string("ab"));
  assert(hash_string("101010") != hash_string("010101"));
}


int main(int argc, char** argv) {
  test_hash_string();
  return 0;
}
