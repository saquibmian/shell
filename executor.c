#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"

void execute_command( struct command_t* command, int* pipes, int input_fd )
{
	FILE* file_in = NULL;
	FILE* file_out = NULL;

	// First command
	if( command->isFirstCommand == true && command->isLastCommand == false && input_fd == 0 ) 
	{
		dup2( pipes[PIPE_WRITE], STDOUT_FILENO );
	} 
	// Middle commands
	else if( command->isFirstCommand == false && command->isLastCommand == false && input_fd != 0 ) 
	{
		dup2( input_fd, STDIN_FILENO );
		dup2( pipes[PIPE_WRITE], STDOUT_FILENO );
	} 
	// Last command
	else 
	{
		dup2( input_fd, STDIN_FILENO );
	}

	// stdin is redirected
	if( command->redirect_in )
	{
		file_in = fopen( command->redirect_in, "r" );
		dup2( fileno( file_in ), STDIN_FILENO );
	}
	
	// stdout is redirected
	if( command ->redirect_out )
	{
		file_out = fopen( command->redirect_out, "w+" );
		dup2( fileno( file_out ), STDOUT_FILENO );
	}

	// Execute the command
	int result = execvp( command->args[0], command->args );

	// Close any open file streams
	if( file_in != NULL ) fclose( file_in );
	if( file_out != NULL ) fclose( file_out );

	// Exit quietly if failure
	if( result == -1 ) {
		_exit( EXIT_FAILURE );
	}
}
