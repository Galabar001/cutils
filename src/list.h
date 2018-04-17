#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"
#include "generic.h"


typedef struct list_element {
  struct list_element* next;
  generic_value_t value;
} list_element_t;

typedef struct {
  void (*value_deallocator)(void*);
  list_element_t* head;
  list_element_t* tail;
  size_t size;
} list_t;

typedef struct {
  list_t* list;
  list_element_t** current;
} list_iterator_t;


/**
 * Creates a new list.
 *
 * Args:
 *  list: Set to the newly allocated list.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not create list because of memory error.
 */
error_t list_create(list_t** list);


/**
 * Creates a new list with a value deallocator.
 *
 * The deallocation function is used to delete void* values 
 * (generic_value_t.p) when list_delete is called.
 *
 * Args:
 *  list: Set to the newly allocated list.
 *  deallocated: Function used to delete values.
 *
 * Returns:
 *  0 on success.
 *  ERROR_OUT_OF_MEMORY: Could not create list because of memory error.
 */
error_t list_create_with_value_deallocator(
  list_t** list, void (*value_deallocator)(void*));


/**
 * Deletes a list.
 *
 * If a deallocation function was provided during creation, list values
 * will be treated as void* (generic_value_t.p) and sent to the function.
 *
 * Args:
 *  list: List to be deleted.
 */
void list_delete(list_t* list);


/**
 * Adds a value to the end of the list.
 *
 * Args:
 *  list: List to add value to.
 *  value: Value to add to list.
 *
 * Returns:
 *  0 on success
 *  ERROR_OUT_OF_MEMORY: Could not add value because of memory error.
 */
error_t list_push_back(list_t* list, generic_value_t value);


/**
 * Removes the last element from the list.
 *
 * Calling this on an empty list is undefined.
 *
 * Args:
 *  list: The list to pop element from.
 *
 * Returns:
 *  The last element in the list.
 */
generic_value_t list_pop_back(list_t* list);


/**
 * Gets the last value in the list.
 *
 * Calling this on an empty list is undefined.
 *
 * Args:
 *  list: The list to examine.
 *
 * Returns:
 *  This last value in the list.
 */
inline generic_value_t list_get_back(list_t* list) {
  return list->tail->value;
}


/**
 * Adds a value to the beginning of the list.
 *
 * Args:
 *  list: List to add value to.
 *  value: Value to add to list.
 *
 * Returns:
 *  0 on success
 *  ERROR_OUT_OF_MEMORY: Could not add value because of memory error.
 */
error_t list_push_front(list_t* list, generic_value_t value);


/**
 * Removes the first element from the list.
 *
 * Calling this on an empty list is undefined.
 *
 * Args:
 *  list: The list to pop element from.
 *
 * Returns:
 *  The first element in the list.
 */
generic_value_t list_pop_front(list_t* list);


/**
 * Gets the first value in the list.
 *
 * Calling this on an empty list is undefined.
 *
 * Args:
 *  list: The list to examine.
 *
 * Returns:
 *  This first value in the list.
 */
inline generic_value_t list_get_front(list_t* list) {
  return list->head->value;
}


/**
 * Returns the size of the list.
 *
 * Args:
 *  list: List to examine.
 *
 * Returns:
 *  Size of the list.
 */
inline size_t list_size(const list_t* list) { return list->size; }


/**
 * Returns true if list is empty.
 *
 * Args:
 *  list: List to examine.
 *
 * Returns:
 *  true if list is empty.
 */
inline bool list_empty(const list_t* list) { return !list->size; }


/**
 * Creates an iterator for a list.
 *
 * Args:
 *  list: List to create iterator for.
 *
 * Returns:
 *  Iterator for the given list.
 */
inline list_iterator_t list_iterator_create(list_t* list) {
  return (list_iterator_t){list, &list->head};
}


/**
 * Returns true if the iterator has a current value.
 *
 * If this function returns true, it is safe to call
 * list_iterator_get_current and list_iterator_next.
 *
 * Args:
 *  iter: Iterator to examine
 *
 * Returns:
 *  true if the iterator has a current value.
 */
inline bool list_iterator_has_current(list_iterator_t* iter) {
  return *iter->current;
}


/**
 * Gets the current value for the iterator.
 *
 * This call should be proceeded by a successful call to
 * list_iterator_has_current.
 *
 * Args:
 *  iter: Iterator to examine.
 *
 * Returns.
 *  Current value for the iterator.
 */
inline generic_value_t list_iterator_get_current(list_iterator_t* iter) {
  return (*iter->current)->value;
}


/**
 * Removes the current value of the iterator.
 *
 * This call should be proceeded by a successful call to
 * list_iterator_has_current.
 *
 * After this call returns, the iterator will be positioned at
 * the next element in the list.
 *
 * Args:
 *  iter: Iterator to examine.
 *
 * Returns:
 *  Current value for the iterator.
 */
generic_value_t list_iterator_remove_current(list_iterator_t* iter);


/**
 * Moves the iterator to the next value.
 *
 * This call should be proceeded by a successful call to
 * list_iterator_has_current.
 *
 * Args:
 *  iter: Iterator to update.
 */
inline void list_iterator_next(list_iterator_t* iter) {
  iter->current = &(*iter->current)->next;
}
