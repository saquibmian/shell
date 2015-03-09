#include "command.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "history.h"
#include "executor.h"
#include "constants.h"

static int num_forks = 0;
 
static void wait_for_forks()
{
	int i;
	for ( i = 0; i < num_forks; ++i) wait( NULL ); 
}

static int run_command( struct command_t* command, int input_fd )
{
	++num_forks;

	int pipes[2];
	pipe( pipes );	

	pid_t pid = fork(); 
	if ( pid == 0 ) // if child process
	{
		execute_command( command, pipes, input_fd );
	}
 
	// Close all open writeable streams
	close( pipes[PIPE_WRITE] );
	if ( input_fd != 0 ) close( input_fd );
 
	// If it's the last command
	if( command->isLastCommand == true ) 
	{
		close( pipes[PIPE_READ] );
		wait_for_forks();
	}
 
	return pipes[PIPE_READ];
}

static inline void internal_command_exit( struct command_t* command )
{
	int exit_code = 0;
	
	// Grab the exit code, if provided
	if( command->args[1] != NULL )
	{
		exit_code = atoi( command->args[1] );
	}

	exit( exit_code );	
}

static inline void internal_command_history( struct command_t* command )
{
	if( command->args[1] != NULL )
	{
		print_history( atoi( command->args[1] ) );
	}
	else
	{
		print_history( 0 );
	}
}

static bool handle_internal_command( struct command_t* command )
{
	// exit
	if (strcmp( command->args[0], "exit" ) == 0 ) 
	{
		internal_command_exit( command );
		return true;
	}

	// history
	if (strcmp( command->args[0], "history" ) == 0 ) 
	{
		internal_command_history( command );
		return true;
	}

	// not an internal command
	return false;
}

int run( struct command_t* command, int input )
{
	parse_command( command );

	if ( command->command != NULL ) 
	{
		if( !handle_internal_command( command ) )
		{
			return run_command( command, input );			
		}
	}
	return 0;
} 