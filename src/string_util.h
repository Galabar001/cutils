#pragma once

#include "errors.h"


/**
 * Copys a string using malloc.
 *
 * Args:
 *  s: String to be copied.
 *  result: Set to the newly allocated string.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not allocate new string.
 */
error_t string_copy(const char* s, char** result);
