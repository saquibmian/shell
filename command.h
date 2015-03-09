#ifndef command_h
#define command_h

#include "bool.h"

// Simple structure to hold the command information
struct command_t 
{
	char* command;
	char* args[512];
	char* redirect_out;
	char* redirect_in;
	bool isFirstCommand;
	bool isLastCommand;
};

// Execute a command; this method takes care of the forking and creation of pipes
int run( struct command_t* command, int input );

#endif