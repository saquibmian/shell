#ifndef list_h
#define list_h

#include <stddef.h>

struct array_list 
{
    int capacity;
    int size;
    const void** elements;
};

typedef struct array_list List;

// initializes the list to an empty list at an initial capacity
void list_init( List** list, int initial_capacity, size_t size_of_element );

// adds an item to the back of list
void list_add( List* list, const void* item );

// remove the item at the specified index
void list_remove( List* list, int index );

// returns the item at the index
const void* list_at( List* list, int index );

// frees the memory used by the list
void list_free( List** list );

// frees all of the elements in the list
void list_free_elements( List* list );

#endif