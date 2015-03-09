#include "parser.h"

#include <ctype.h>
#include <string.h>

static inline char* skip_whitespace( char* s )
{
	while( isspace( *s ) ) ++s;
	return s;
}
 
void parse_command( struct command_t* command )
{
	char* token = command->command;

	bool nextIsRedirectOut = false;
	bool nextIsRedirectIn = false;

	// split the command by spaces
	int num_tokens = 0;
	token = skip_whitespace( token );
	char* next = strchr( token, ' ' );
	while( next != NULL ) {
		next[0] = '\0';

		// If redirecting out, the next arg is the out stream
		if( strcmp( token, ">" ) == 0 )
		{
			nextIsRedirectOut = true;
			token = skip_whitespace( next + 1 );
			next = strchr( token, ' ' );
			continue;
		}

		// If redirecting in, the next arg is the in stream
		if( strcmp( token, "<" ) == 0 )
		{
			nextIsRedirectIn = true;
			token = skip_whitespace( next + 1 );
			next = strchr( token, ' ' );
			continue;
		}

		if( nextIsRedirectOut == true )
		{
			command->redirect_out = token;
			nextIsRedirectOut = false;
		}
		else if ( nextIsRedirectIn == true )
		{
			command->redirect_in = token;
			nextIsRedirectIn = false;
		}
		else 
		{
			command->args[num_tokens] = token;			
			++num_tokens;
		}

		token = skip_whitespace( next + 1 );
		next = strchr( token, ' ' );
	}
 
	// handle the last arg, if there is one
	if ( token[0] != '\0' ) {
		if( nextIsRedirectOut == true )
		{
			command->redirect_out = token;
			nextIsRedirectOut = false;
		}
		else if ( nextIsRedirectIn == true )
		{
			command->redirect_in = token;
			nextIsRedirectIn = false;
		}
		else 
		{
			command->args[num_tokens] = token;			
			++num_tokens;
		}

		// replace newline character with string terminator
		next = strchr( token, '\n' );
		next[0] = '\0';
		++num_tokens; 
	}
 
	// indicate end of args list
	command->args[num_tokens] = NULL;
}
