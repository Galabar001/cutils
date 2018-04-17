
#include <assert.h>
#include <stdlib.h>

#include "list.h"


static void test_list_create() {
  list_t* list;
  assert(!list_create(&list));
  assert(!list->value_deallocator);
  assert(!list->head);
  assert(!list->tail);
  assert(!list->size);
  list_delete(list);
}


static void test_list_delete() {
  list_t* list;
  assert(!list_create(&list));
  list_delete(list);  
}


static void test_list_push_back() {
  list_t* list;
  assert(!list_create(&list));
  for (uint64_t i = 0; i < 10; i++) {
    assert(!list_push_back(list, (generic_value_t)i));
  }
  assert(10 == list_size(list));

  uint64_t value = 0;
  list_iterator_t iter = list_iterator_create(list);
  while (list_iterator_has_current(&iter)) {
    assert(value == list_iterator_get_current(&iter).i64);
    list_iterator_next(&iter);
    value++;
  }
  list_delete(list);
}


static void test_list_push_front() {
  list_t* list;
  assert(!list_create(&list));
  for (uint64_t i = 0; i < 10; i++) {
    assert(!list_push_front(list, (generic_value_t)i));
  }
  assert(10 == list_size(list));
  
  uint64_t value = 9;
  list_iterator_t iter = list_iterator_create(list);
  while (list_iterator_has_current(&iter)) {
    assert(value == list_iterator_get_current(&iter).i64);
    list_iterator_next(&iter);
    value--;
  }
  list_delete(list);
}


static void test_list_pop_front() {
  list_t* list;
  assert(!list_create(&list));
  for (uint64_t i = 0; i < 10; i++) {
    assert(!list_push_front(list, (generic_value_t)i));
  }

  uint64_t value = 9;
  while (!list_empty(list)) {
    assert(value == list_pop_front(list).i64);
    value--;
  }
  assert(!list_size(list));
  list_delete(list);
}


static void test_list_with_value_deallocator() {
  list_t* list;
  assert(!list_create_with_value_deallocator(&list, free));
  for (uint64_t i = 0; i < 10; i++) {
    void* p = malloc(100);
    assert(p);
    assert(!list_push_front(list, (generic_value_t)p));
  }
  assert(10 == list_size(list));
  list_delete(list);
}


static void test_list_iterator_remove_current() {
  list_t* list;
  assert(!list_create(&list));
  for (uint64_t i = 0; i < 10; i++) {
    assert(!list_push_back(list, (generic_value_t)i));
  }

  for (list_iterator_t iter = list_iterator_create(list);
       list_iterator_has_current(&iter); ) {
    // Remove odd numbers.
    if (list_iterator_get_current(&iter).i64 % 2) {
      (void)list_iterator_remove_current(&iter);
    } else {
      list_iterator_next(&iter);
    }
  }
  assert(0 == list_get_front(list).i64);
  assert(8 == list_get_back(list).i64);
  assert(5 == list_size(list));
  // Check for the even values.
  uint64_t expected = 0;
  for (list_iterator_t iter = list_iterator_create(list);
       list_iterator_has_current(&iter); list_iterator_next(&iter)) {
    assert(expected == list_iterator_get_current(&iter).i64);
    expected += 2;
  }
  list_delete(list);  
}

static void test_list_iterator_remove_current_single_element() {
  list_t* list;
  assert(!list_create(&list));
  assert(!list_push_front(list, (generic_value_t)(uint64_t)0));
  list_iterator_t iter = list_iterator_create(list);
  list_iterator_remove_current(&iter);
  assert(!list_iterator_has_current(&iter));
  assert(!list_size(list));
  assert(!list->head);
  assert(!list->tail);
  list_delete(list);  
}


static void test_list_iterator_remove_current_two_elements_remove_first() {
  list_t* list;
  assert(!list_create(&list));
  assert(!list_push_back(list, (generic_value_t)(uint64_t)0));
  assert(!list_push_back(list, (generic_value_t)(uint64_t)1));  
  list_iterator_t iter = list_iterator_create(list);
  list_iterator_remove_current(&iter);
  assert(list_iterator_has_current(&iter));
  assert(1 == list_iterator_get_current(&iter).i64);
  assert(1 == list_get_front(list).i64);
  assert(1 == list_get_back(list).i64);  
  assert(1 == list_size(list));
  assert(list->head == list->tail);  
  list_delete(list);  
}


static void test_list_iterator_remove_current_two_elements_remove_second() {
  list_t* list;
  assert(!list_create(&list));
  assert(!list_push_back(list, (generic_value_t)(uint64_t)0));
  assert(!list_push_back(list, (generic_value_t)(uint64_t)1));
  list_iterator_t iter = list_iterator_create(list);
  list_iterator_next(&iter);
  list_iterator_remove_current(&iter);
  assert(!list_iterator_has_current(&iter));
  assert(0 == list_get_front(list).i64);
  assert(0 == list_get_back(list).i64);
  assert(1 == list_size(list));
  assert(list->head == list->tail);
  list_delete(list);  
}


int main(int argc, char** argv) {
  test_list_create();
  test_list_delete();
  test_list_push_back();
  test_list_push_front();
  test_list_pop_front();
  test_list_with_value_deallocator();
  test_list_iterator_remove_current();
  test_list_iterator_remove_current_single_element();  
  test_list_iterator_remove_current_two_elements_remove_first();
  test_list_iterator_remove_current_two_elements_remove_second();
}
