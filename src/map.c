#include "map.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "string_util.h"


static const size_t INITIAL_CAPACITY = 10;


// Provide external definitions of inline functions.
extern inline size_t map_size(const map_t* map);
extern inline bool map_iterator_has_current(map_iterator_t* iter);
extern inline void map_iterator_get_current(
  map_iterator_t* iter, const char** key, generic_value_t* value);


error_t map_create(map_t** map) {
  return map_create_with_value_deallocator(map, 0);
}


error_t map_create_with_value_deallocator(
  map_t** map, void (*value_deallocator)(void*)) {
  map_t* tmp = calloc(1, sizeof(map_t));
  if (!tmp) {
    return ERROR_OUT_OF_MEMORY;
  }
  tmp->value_deallocator = value_deallocator;
  tmp->capacity = INITIAL_CAPACITY;
  tmp->buckets = calloc(tmp->capacity, sizeof(list_t*));
  if (!tmp->buckets) {
    free(tmp);
    return ERROR_OUT_OF_MEMORY;
  }
  *map = tmp;
  return 0;
}


void map_delete(map_t* map) {
  if (!map) {
    return;
  }
  for (size_t i = 0; i < map->capacity; i++) {
    if (map->buckets[i]) {
      list_delete(map->buckets[i]);
    }
  }
  free(map->buckets);
  free(map);
}


static bool map_find_element(
  list_t* bucket, const char* key, list_iterator_t* iter) {
  for (list_iterator_t iter_tmp = list_iterator_create(bucket);
       list_iterator_has_current(&iter_tmp); list_iterator_next(&iter_tmp)) {
    map_element_t* tmp =
      (map_element_t*)list_iterator_get_current(&iter_tmp).p;
    if (!strcmp(tmp->key, key)) {
      *iter = iter_tmp;
      return true;
    }
  }
  return false;
}


static void map_element_delete(map_element_t* element) {
  if (!element) {
    return;
  }
  free(element->key);
  free(element);
}


static void map_element_deallocator(map_element_t* element) {
  if (element->value_deallocator) {
    element->value_deallocator(element->value.p);
  }
  map_element_delete(element);
}


error_t map_insert(map_t* map, const char* key, generic_value_t value) {
  // Find the associated bucket.
  uint64_t hash_code = hash_string(key);
  size_t index = hash_code % map->capacity;
  list_t* bucket = map->buckets[index];
  bool new_bucket = false;
  if (!bucket) {
    error_t error = list_create_with_value_deallocator(
      &bucket, (void(*)(void*))map_element_deallocator);
    if (error) {
      return error;
    }
    map->buckets[index] = bucket;
    new_bucket = true;
  } else {
    // Check to see if the key is already present.
    list_iterator_t iter;
    if (map_find_element(bucket, key, &iter)) {
      map_element_t* element = (map_element_t*)
	list_iterator_get_current(&iter).p;
      element->value = value;
      return 0;
    }
  }

  // Add new value.
  map_element_t* element = calloc(1, sizeof(map_element_t));
  if (!element) {
    goto out_of_memory;
  }

  if (string_copy(key, &element->key)) {
    goto out_of_memory;
  }
  element->hash_code = hash_code;
  element->value = value;
  element->value_deallocator = map->value_deallocator;

  if (list_push_back(bucket, (generic_value_t)(void*)element)) {
    goto out_of_memory;
  }
  map->size++;
  return 0;

 out_of_memory:
  map_element_delete(element);
  if (new_bucket) {
    list_delete(bucket);
    map->buckets[index] = 0;
  }
  return ERROR_OUT_OF_MEMORY;
}


bool map_get(map_t* map, const char* key, generic_value_t* value) {
  uint64_t hash_code = hash_string(key);
  size_t index = hash_code % map->capacity;
  list_t* bucket = map->buckets[index];
  if (bucket) {
    list_iterator_t iter;
    if (map_find_element(bucket, key, &iter)) {
      map_element_t* element = (map_element_t*)
	list_iterator_get_current(&iter).p;
      *value = element->value;
      return true;
    }
  }
  return false;
}


bool map_remove(map_t* map, const char* key, generic_value_t* value) {
  uint64_t hash_code = hash_string(key);
  size_t index = hash_code % map->capacity;
  list_t* bucket = map->buckets[index];
  if (bucket) {
    list_iterator_t iter;
    if (map_find_element(bucket, key, &iter)) {
      map_element_t* element = (map_element_t*)
	list_iterator_remove_current(&iter).p;
      *value = element->value;
      map_element_delete(element);
      map->size--;

      // Delete empty bucket.
      if (!list_size(bucket)) {
	list_delete(bucket);
	map->buckets[index] = 0;
      }
      return true;
    }
  }
  return false;
}


static void map_iterator_find_bucket(map_iterator_t* iter) {
  while (iter->bucket_index < iter->map->capacity) {
      list_t* bucket = iter->map->buckets[iter->bucket_index];
      if (bucket) {
	iter->bucket_iter = list_iterator_create(bucket);
	break;
      }
      iter->bucket_index++;    
  }
}


map_iterator_t map_iterator_create(map_t* map) {
  map_iterator_t iter = {map, 0};
  // Find the first bucket.
  map_iterator_find_bucket(&iter);
  return iter;
}


generic_value_t map_iterator_remove_current(map_iterator_t* iter) {
  generic_value_t value = list_iterator_remove_current(&iter->bucket_iter);
  map_element_delete((map_element_t*)value.p);
  if (!list_iterator_has_current(&iter->bucket_iter)) {
    // Delete empty bucket.
    list_t* bucket = iter->map->buckets[iter->bucket_index];
    if (!list_size(bucket)) {
      list_delete(bucket);
      iter->map->buckets[iter->bucket_index] = 0;
    }
    // Find the next bucket.    
    iter->bucket_index++;
    map_iterator_find_bucket(iter);
  }
  iter->map->size--;
  return value;
}


void map_iterator_next(map_iterator_t* iter) {
  list_iterator_next(&iter->bucket_iter);
  if (!list_iterator_has_current(&iter->bucket_iter)) {
    // Find the next bucket.
    iter->bucket_index++;
    map_iterator_find_bucket(iter);
  }
}
