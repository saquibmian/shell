#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

void expand_capacity( List* list );

void list_init( List** list, int initial_capacity, size_t size_of_element )
{
    List* new_list = malloc( sizeof( List ) );
    new_list->elements = malloc( initial_capacity * size_of_element );
    new_list->capacity = initial_capacity;
    new_list->size = 0;

    *list = new_list;
}

void list_add( List* list, const void* item ) 
{
    if( list->capacity == list->size )
    {
        expand_capacity( list );
    }
    list->elements[list->size] = item;
    ++( list->size );
}

void list_remove( List* list, int index ) 
{
    // simply move the memory over by one unit
    int i;
    for ( i = index; i < list->size; ++i )
    {
        list->elements[i] = list->elements[i+1];
    }
    --( list->size );
}

const void* list_at( List* list, int index )
{
    if( index >= list->size )
    {
        syslog( LOG_ERR, "Error: invalid index in list: %d\n", index );
        return NULL;
    }
    return list->elements[index];
}

// doubles the capacity of the allocated memory
void expand_capacity( List* list )
{
    size_t new_size = sizeof( list->elements ) * list->capacity * 2;
    list->elements = realloc( list->elements, new_size );
    list->capacity *= 2;
}

void list_free( List** list )
{
    free( (*list)->elements );
    free( *list );
}

void list_free_elements( List* list )
{
    for( int i = 0; i < list->size; ++i )
    {
        void* element  = (void*) list_at( list, i );
        free( element );
    }
}
