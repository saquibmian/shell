#include "history.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "constants.h"

static List* history;

void init_history()
{
	list_init( &history, NUM_DEFAULT_HISTORY_STORAGE, sizeof( char ) * MAX_LINE_LENGTH );
}

char* create_history_item( char* command )
{
	char* history_item = malloc( sizeof( char ) * MAX_LINE_LENGTH );
	memcpy( history_item, command, sizeof( char ) * MAX_LINE_LENGTH );

	return history_item;
}

void add_history_item( char* history_item )
{
	list_add( history, history_item );
}

void print_history( int count )
{
	int history_count = NUM_DEFAULT_HISTORY_PRINT;
	if( count > 0 )
	{
		history_count = count;
	}

	int i = history->size - history_count;
	if(  i < 0 ) i = 0;
    for( ; i < history->size; ++i )
    {
        char* line  = (char*) list_at( history, i );
        printf( "%s", line );
    }
}

void cleanup_history() 
{
	list_free_elements( history );
	list_free( &history );
}

