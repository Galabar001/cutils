#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"
#include "generic.h"
#include "list.h"


typedef struct {
  char* key;
  uint64_t hash_code;
  generic_value_t value;
  void (*value_deallocator)(void*);
} map_element_t;

typedef struct {
  void (*value_deallocator)(void*);
  size_t size;
  size_t capacity;
  list_t** buckets;
} map_t;

typedef struct {
  map_t* map;
  size_t bucket_index;
  list_iterator_t bucket_iter;
} map_iterator_t;
  

/**
 * Creates a new map.
 *
 * Args:
 *  map: Set to the newly allocated map.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not create map because of memory error.
 */
error_t map_create(map_t** map);


/**
 * Creates a new map with a value deallocator.
 *
 * The deallocation function is used to delete void* values 
 * (generic_value_t.p) when map_delete is called.
 *
 * Args:
 *  map: Set to the newly allocated map.
 *  deallocated: Function used to delete values.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not create map because of memory error.
 */
error_t map_create_with_value_deallocator(
  map_t** map, void (*value_deallocator)(void*));


/**
 * Deletes a map.
 *
 * If a deallocation function was provided during creation, map values
 * will be treated as void* (generic_value_t.p) and sent to the function.
 *
 * Args:
 *  map: Map to be deleted.
 */
void map_delete(map_t* map);


/**
 * Inserts a key and value into the map.
 *
 * If the key already exists in the map, the value is updated.
 *
 * Args:
 *  map: Map to update.
 *  key: Key for map entry.
 *  value: Value for map entry.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not insert into map because of memory errors.
 */
error_t map_insert(map_t* map, const char* key, generic_value_t value);


/**
 * Gets a value from the map.
 *
 * Args:
 *  map: The map to examine.
 *  key: The key to look up.
 *  value: Set to any value found.
 *
 * Returns:
 *  true if the value is found.
 */
bool map_get(map_t* map, const char* key, generic_value_t* value);


/**
 * Removes a key and value from the map.
 *
 * Args:
 *  map: The map to examine.
 *  key: The key to look up.
 *  value: Set to any value found.
 *
 * Returns:
 *  true if the value is found.
 */
bool map_remove(map_t* map, const char* key, generic_value_t* value);


/**
 * Returns the size of the map.
 *
 * Args:
 *  map: The map to examine.
 *
 * Returns:
 *  Size of the map.
 */
inline size_t map_size(const map_t* map) { return map->size; }


/**
 * Creates an iterator for a map.
 *
 * Args:
 *  map: Map to create iterator for.
 *
 * Returns:
 *  Iterator for the given map.
 */
map_iterator_t map_iterator_create(map_t* map);


/**
 * Returns true if the iterator has a current element.
 *
 * If this function returns true, it is safe to call
 * map_iterator_get_current and map_iterator_next.
 *
 * Args:
 *  iter: Iterator to examine
 *
 * Returns:
 *  true if the iterator has a current element.
 */
inline bool map_iterator_has_current(map_iterator_t* iter) {
  return iter->bucket_index < iter->map->capacity;
}


/**
 * Gets the current element for the iterator.
 *
 * This call should be proceeded by a successful call to
 * map_iterator_has_current.
 *
 * Args:
 *  iter: Iterator to examine.
 *  key: Set to the key for the current element (owned by map and only valid
 *   while map has not been modified).
 *  value: Set to the value for the current element.
 *
 * Returns.
 *  Current element for the iterator.
 */
inline void map_iterator_get_current(
  map_iterator_t* iter, const char** key, generic_value_t* value) {
  map_element_t* element =
    (map_element_t*)list_iterator_get_current(&iter->bucket_iter).p;
  *key = element->key;
  *value = element->value;
}


/**
 * Removes the current element value of the iterator.
 *
 * This call should be proceeded by a successful call to
 * map_iterator_has_current.
 *
 * After this call returns, the iterator will be positioned at
 * the next element in the map.
 *
 * Args:
 *  iter: Iterator to examine.
 *
 * Returns:
 *  Current element value for the iterator.
 */
generic_value_t map_iterator_remove_current(map_iterator_t* iter);


/**
 * Moves the iterator to the next value.
 *
 * This call should be proceeded by a successful call to
 * map_iterator_has_current.
 *
 * Args:
 *  iter: Iterator to update.
 */
void map_iterator_next(map_iterator_t* iter);
