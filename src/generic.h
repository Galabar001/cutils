#pragma once

#include <stdint.h>


typedef union {
  int64_t i64;
  uint64_t ui64;
  double d;
  void* p;
} generic_value_t;

