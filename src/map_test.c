#include "map.h"

#include <assert.h>


static void test_map_create() {
  map_t* map;
  assert(!map_create(&map));
  assert(!map_size(map));
  map_delete(map);
}


static void test_map_delete() {
  map_t* map;
  assert(!map_create(&map));
  map_delete(map);
}


static void test_map_insert() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }
  assert(10 == map_size(map));
  map_delete(map);
}


static void test_map_get() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    generic_value_t value;
    assert(map_get(map, key, &value));
    assert(key[0] == value.i64 + 1);
    assert(i == value.i64);
  }
  map_delete(map);
}


static void test_map_remove() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    generic_value_t value;
    assert(map_remove(map, key, &value));
    assert(key[0] == value.i64 + 1);
    assert(i == value.i64);
  }
  assert(!map_size(map));
  map_delete(map);
}


static void test_map_iterator_create() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  map_iterator_t iter = map_iterator_create(map);
  assert(iter.bucket_index >= 0);
  assert(iter.bucket_index < map->capacity);
  map_delete(map);
}


static void test_map_iterator_has_current() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  map_iterator_t iter = map_iterator_create(map);
  assert(map_iterator_has_current(&iter));
  map_delete(map);  
}


static void test_map_iterator_get_current() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  map_iterator_t iter = map_iterator_create(map);
  const char* key;
  generic_value_t value;
  map_iterator_get_current(&iter, &key, &value);

  generic_value_t value_found;
  assert(map_get(map, key, &value_found));
  assert(value.i64 == value_found.i64);
  map_delete(map);
}


static void test_map_iterator_next() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  // We iterate through the map, making sure that we see every value once.
  map_t* seen;
  assert(!map_create(&seen));
  for (map_iterator_t iter = map_iterator_create(map);
       map_iterator_has_current(&iter); map_iterator_next(&iter)) {
    const char* key;
    generic_value_t value;
    map_iterator_get_current(&iter, &key, &value);
    assert(key[0] == value.i64 + 1);

    generic_value_t other_value;
    assert(!map_get(seen, key, &other_value));
    assert(!map_insert(seen, key, value));
  }
  assert(map_size(map) == map_size(seen));
  map_delete(seen);
  map_delete(map);
}


static void test_map_iterator_remove_current() {
  map_t* map;
  assert(!map_create(&map));
  for (uint64_t i = 0; i < 10; i++) {
    char key[2] = {(char)i + 1};
    assert(!map_insert(map, key, (generic_value_t)i));
  }

  for (map_iterator_t iter = map_iterator_create(map);
       map_iterator_has_current(&iter); ) {
    const char* key;
    generic_value_t value;
    map_iterator_get_current(&iter, &key, &value);
    // Remove odd numbers.
    if (value.i64 % 2) {
      (void)map_iterator_remove_current(&iter);
    } else {
      map_iterator_next(&iter);
    }
  }
  assert(5 == map_size(map));
  // Check for the even values.
  for (uint64_t i = 0; i < 5; i++) {
    char key[2] = {(char)(i * 2) + 1};
    generic_value_t value;
    assert(map_get(map, key, &value));
    assert(i * 2 == value.i64);
  }
  map_delete(map);
}


static void test_map_iterator_empty_list() {
  map_t* map;
  assert(!map_create(&map));
  map_iterator_t iter = map_iterator_create(map);
  assert(!map_iterator_has_current(&iter));
  map_delete(map);  
}


int main(int argc, char** argv) {
  test_map_create();
  test_map_delete();
  test_map_insert();
  test_map_get();
  test_map_remove();
  test_map_iterator_create();
  test_map_iterator_has_current();
  test_map_iterator_get_current();
  test_map_iterator_next();
  test_map_iterator_remove_current();
  test_map_iterator_empty_list();
  return 0;
}
