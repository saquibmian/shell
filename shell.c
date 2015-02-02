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
		// Show the prompt
		prompt();
 
		// Read a command
		fgets( line, sizeof( line ) / sizeof( char ), stdin );
		char* history_item = create_history_item( line );
		
		// Split by | and run each command individually
		int input_stream = 0;
		bool isFirstCommand = true; 
		char* cmd = line;
		char* next_pipe_delimiter = strchr( cmd, '|' );
		while ( next_pipe_delimiter != NULL ) {
			*next_pipe_delimiter = '\0'; // used to point to '|'
			struct command_t command = {
				.command = cmd,
				.isFirstCommand = isFirstCommand,
				.isLastCommand = false
			};
			input_stream = run( &command, input_stream );
 
			cmd = next_pipe_delimiter + 1;
			next_pipe_delimiter = strchr( cmd, '|' );
			isFirstCommand = false;
		}

		// Run the last command
		struct command_t command = {
			.command = cmd,
			.isFirstCommand = isFirstCommand,
			.isLastCommand = true
		};
		input_stream = run( &command, input_stream );

		add_history_item( history_item );
	}

	cleanup_history();
	
	return EXIT_SUCCESS;
}
