#include "parser.h"

#include <ctype.h>
#include <string.h>

static char* skip_whitespace( char* s )
{
	while( isspace( *s ) ) ++s;
	return s;
}
 
void parse_command( struct command_t* command )
{
	char* cmd = command->command;

	bool nextIsRedirectOut = false;
	bool nextIsRedirectIn = false;

	// split the command by spaces
	int i = 0;
	cmd = skip_whitespace( cmd );
	char* next = strchr( cmd, ' ' );
	while( next != NULL ) {
		next[0] = '\0';

		if( strcmp( cmd, ">" ) == 0 )
		{
			nextIsRedirectOut = true;
			cmd = skip_whitespace( next + 1 );
			next = strchr( cmd, ' ' );
			continue;
		}

		if( strcmp( cmd, "<" ) == 0 )
		{
			nextIsRedirectIn = true;
			cmd = skip_whitespace( next + 1 );
			next = strchr( cmd, ' ' );
			continue;
		}

		if( nextIsRedirectOut == true )
		{
			command->redirect_out = cmd;
			nextIsRedirectOut = false;
		}
		else if ( nextIsRedirectIn == true )
		{
			command->redirect_in = cmd;
			nextIsRedirectIn = false;
		}
		else 
		{
			command->args[i] = cmd;			
			++i;
		}

		cmd = skip_whitespace( next + 1 );
		next = strchr( cmd, ' ' );
	}
 
	// handle the last arg, if there is one
	if ( cmd[0] != '\0' ) {
		if( nextIsRedirectOut == true )
		{
			command->redirect_out = cmd;
			nextIsRedirectOut = false;
		}
		else if ( nextIsRedirectIn == true )
		{
			command->redirect_in = cmd;
			nextIsRedirectIn = false;
		}
		else 
		{
			command->args[i] = cmd;			
			++i;
		}

		// replace newline character with string terminator
		next = strchr( cmd, '\n' );
		next[0] = '\0';
		++i; 
	}
 
	// indicate end of args list
	command->args[i] = NULL;
}