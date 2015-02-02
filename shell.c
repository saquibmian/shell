#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include <pwd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "list.h"
 
#define READ  0
#define WRITE 1
#define MAX_LINE_LENGTH 1024
#define NUM_DEFAULT_COMMANDS 25

typedef enum { false, true } bool;

struct command_t 
{
	char* command;
	char* args[512];
	char* redirect_out;
	char* redirect_in;
	bool isFirstCommand;
	bool isLastCommand;
};
 
static const char* get_username()
{
	struct passwd* pw = getpwuid( geteuid() );
	if ( pw )
	{
		return pw->pw_name;
	}

	return "";
}

static int execute_command( char* commands, char** args )
{
	//char* redirect_out = strchr( )
	return 0;
}
 
/*
 * Handle commands separatly
 * input: return value from previous command (useful for pipe file descriptor)
 * first: 1 if first command in pipe-sequence (no input from previous pipe)
 * last: 1 if last command in pipe-sequence (no input from previous pipe)
 *
 * EXAMPLE: If you type "ls | grep shell | wc" in your shell:
 *    fd1 = command(0, 1, 0), with args[0] = "ls"
 *    fd2 = command(fd1, 0, 0), with args[0] = "grep" and args[1] = "shell"
 *    fd3 = command(fd2, 0, 1), with args[0] = "wc"
 *
 * So if 'command' returns a file descriptor, the next 'command' has this
 * descriptor as its 'input'.
 */
static int run_command( struct command_t* command, int input )
{
	/* Invoke pipe */
	int pipettes[2];
	pipe( pipettes );	
	pid_t pid = fork();
 
	/*
	 SCHEME:
	 	STDIN --> O --> O --> O --> STDOUT
	*/
 
	if ( pid == 0 ) // if child process
	{
		FILE* file_in;
		FILE* file_out;

		if( command->isFirstCommand == true && command->isLastCommand == false && input == 0 ) {
			// First command
			dup2( pipettes[WRITE], STDOUT_FILENO );
		} 
		else if( command->isFirstCommand == false && command->isLastCommand == false && input != 0 ) 
		{
			// Middle command
			dup2( input, STDIN_FILENO );
			dup2( pipettes[WRITE], STDOUT_FILENO );
		} 
		else 
		{
			// Last command
			dup2( input, STDIN_FILENO );
		}

		// '<' overwrides all pipes
		if( command->redirect_in )
		{
			file_in = fopen( command->redirect_in, "r" );
			dup2( fileno( file_in ), STDIN_FILENO );
		}
		if( command ->redirect_out )
		{
			file_out = fopen( command->redirect_out, "w+" );
			dup2( fileno( file_out ), STDOUT_FILENO );
		}

		int result = execvp( command->args[0], command->args );

		if( command->redirect_in )
		{
			fclose( file_in );
		}
		if( command ->redirect_out )
		{
			fclose( file_out );
		}
 
		if( result == -1 ) {
			_exit( EXIT_FAILURE ); // If child fails
		}
	}
 
	// Close all open writeable streams
	if ( input != 0 ) close( input );
	close( pipettes[WRITE] );
 
	// If it's the last command, nothing more needs to be read
	if( command->isLastCommand == true ) close( pipettes[READ] );
 
	return pipettes[READ];
}
 
/* Final cleanup, 'wait' for processes to terminate.
 *  n : Number of times 'command' was invoked.
 */
static int n = 0; /* number of calls to 'command' */
static void wait_for_commands()
{
	for ( int i = 0; i < n; ++i) wait(NULL); 
}
 
static List* history;

static bool handle_internal_command( struct command_t* command )
{
	// exit
	if (strcmp( command->args[0], "exit" ) == 0 ) exit( 0 );

	// history
	if (strcmp( command->args[0], "history" ) == 0 ) {
		int history_count = 10;
		if( command->args[1] != NULL )
		{
			history_count = atoi( command->args[1] );
		}

		int i = history->size - history_count;
		if(  i < 0 ) i = 0;
	    for( ; i < history->size; ++i )
	    {
	        char* line  = (char*) list_at( history, i );
	        printf("%s", line );
	    }
		return true;
	}

	// not an internal command
	return false;
}

 
static char* skip_whitespace( char* s )
{
	while( isspace( *s ) ) ++s;
	return s;
}
 
static void parse_command( struct command_t* command )
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

static int run( struct command_t* command, int input )
{
	parse_command( command );

	if ( command->command != NULL ) 
	{
		if( !handle_internal_command( command ) )
		{
			++n;
			return run_command( command, input );			
		}
	}
	return 0;
} 

int main()
{
	char line[MAX_LINE_LENGTH];
	const char* username = get_username();

    list_init( &history, NUM_DEFAULT_COMMANDS, sizeof( line ) );


	while ( true ) {
		/* Print the command prompt */
		printf("%s> ", username );
		fflush( NULL );
 
		/* Read a command line */
		if ( !fgets( line, sizeof( line ) / sizeof( char ), stdin ) ) return -1;
		char* history_item = malloc( sizeof( char ) * MAX_LINE_LENGTH );
		memcpy( history_item, line, sizeof( char ) * MAX_LINE_LENGTH );
 
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
		
		// Wait for all commands to finish
		wait_for_commands();
		n = 0;

		list_add( history, history_item );
	}

	list_free_elements( history );
	list_free( &history );
	return 0;
}
 

 
