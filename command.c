#include "command.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "history.h"
#include "executor.h"
#include "constants.h"

static int forks = 0;
 
static void wait_for_forks()
{
	for ( int i = 0; i < forks; ++i) wait( NULL ); 
}

static int run_command( struct command_t* command, int input_fd )
{
	++forks;

	int pipettes[2];
	pipe( pipettes );	

	pid_t pid = fork(); 
	if ( pid == 0 ) // if child process
	{
		execute_command( command, pipettes, input_fd );
	}
 
	// Close all open writeable streams
	if ( input_fd != 0 ) close( input_fd );
	close( pipettes[PIPE_WRITE] );
 
	// If it's the last command
	if( command->isLastCommand == true ) 
	{
		close( pipettes[PIPE_READ] );
		wait_for_forks();
	}
 
	return pipettes[PIPE_READ];
}
 

static bool handle_internal_command( struct command_t* command )
{
	// exit
	if (strcmp( command->args[0], "exit" ) == 0 ) exit( 0 );

	// history
	if (strcmp( command->args[0], "history" ) == 0 ) {
		if( command->args[1] != NULL )
		{
			print_history( atoi( command->args[1] ) );
		}
		else
		{
			print_history( 0 );
		}

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