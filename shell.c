#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prompt.h"
#include "command.h"
#include "history.h"
#include "constants.h"

int main()
{
	char line[MAX_LINE_LENGTH];
	init_history();

	while ( true ) {
		prompt();
 
		/* Read a command line */
		if ( !fgets( line, sizeof( line ) / sizeof( char ), stdin ) ) return -1;
		char* history_item = create_history_item( line );
		
		int input = 0;
		bool isFirstCommand = true;
 
		char* cmd = line;
		char* next_pipe_delimiter = strchr( cmd, '|' ); /* Find first '|' */
 
		// Split by | and run each command individually
		while ( next_pipe_delimiter != NULL ) {
			/* 'next_pipe_delimiter' points to '|' */
			*next_pipe_delimiter = '\0';
			struct command_t command = {
				.command = cmd,
				.isFirstCommand = isFirstCommand,
				.isLastCommand = false
			};
			input = run( &command, input );
 
			cmd = next_pipe_delimiter + 1;
			next_pipe_delimiter = strchr( cmd, '|' ); /* Find next '|' */
			isFirstCommand = false;
		}
		// Run the last command
		struct command_t command = {
			.command = cmd,
			.isFirstCommand = isFirstCommand,
			.isLastCommand = true
		};
		input = run( &command, input );

		add_history_item( history_item );
	}

	cleanup_history();
	
	return EXIT_SUCCESS;
}
