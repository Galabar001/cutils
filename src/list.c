#include "list.h"

#include <stddef.h>
#include <stdlib.h>


// Provide external definitions of inline functions.
extern inline generic_value_t list_get_back(list_t* list);
extern inline generic_value_t list_get_front(list_t* list);
extern inline size_t list_size(const list_t* list);
extern inline bool list_empty(const list_t* list);

extern inline list_iterator_t list_iterator_create(list_t* list);
extern inline bool list_iterator_has_current(list_iterator_t* iter);
extern inline generic_value_t list_iterator_get_current(list_iterator_t* iter);
extern inline void list_iterator_next(list_iterator_t* iter);


error_t list_create(list_t** list) {
  return list_create_with_value_deallocator(list, 0);
}


error_t list_create_with_value_deallocator(
  list_t** list, void (*value_deallocator)(void*)) {
  list_t* tmp = calloc(1, sizeof(list_t));
  if (!tmp) {
    return ERROR_OUT_OF_MEMORY;
  }
  tmp->value_deallocator = value_deallocator;
  *list = tmp;
  return 0;
}


void list_delete(list_t* list) {
  if (!list) {
    return;
  }
  while (list->head) {
    list_element_t* current = list->head;
    if (list->value_deallocator) {
      list->value_deallocator(current->value.p);
    }
    list->head = current->next;
    free(current);
  }
  free(list);
}


static error_t list_element_create(
  list_element_t** element, generic_value_t value) {
  list_element_t* tmp = calloc(1, sizeof(list_element_t));
  if (!tmp) {
    return ERROR_OUT_OF_MEMORY;
  }
  tmp->value = value;
  *element = tmp;
  return 0;
}

error_t list_push_back(list_t* list, generic_value_t value) {
  list_element_t* element;
  error_t result = list_element_create(&element, value);
  if (result) {
    return result;
  }
  if (!list->head) {
    list->head = list->tail = element;
  } else {
    list->tail->next = element;
    list->tail = element;
  }
  list->size++;
  return 0;
}


error_t list_push_front(list_t* list, generic_value_t value) {
  list_element_t* element;
  error_t result = list_element_create(&element, value);
  if (result) {
    return result;
  }
  if (!list->head) {
    list->head = list->tail = element;
  } else {
    element->next = list->head;
    list->head = element;
  }
  list->size++;
  return 0;
}


generic_value_t list_pop_front(list_t* list) {
  generic_value_t value = list->head->value;
  list_element_t* tmp = list->head;
  list->head = list->head->next;
  if (!list->head) {
    list->tail = 0;
  }
  free(tmp);
  list->size--;
  return value;
}


generic_value_t list_iterator_remove_current(list_iterator_t* iter) {
  list_element_t* tmp = *iter->current;
  *iter->current = tmp->next;

  // Check to see if we must update the tail pointer.
  if (iter->list->tail == tmp) {
    // If we removed the only element, set tail to null.
    if (!iter->list->head) {
      iter->list->tail = 0;
    } else {
      // Otherwise, find current element from pointer to its next field.
      iter->list->tail =
	(list_element_t*)((char*)iter->current -
			  offsetof(list_element_t, next));
    }
  }

  generic_value_t value = tmp->value;
  free(tmp);
  iter->list->size--;
  return value;
}
