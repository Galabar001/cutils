#pragma once

#include <stdint.h>


/**
 * Hashes a string value.
 *
 * Args:
 *  s: String to hash.
 *
 * Returns:
 *  Hash value for string.
 */
uint64_t hash_string(const char* s);
