#ifndef command_h
#define command_h

#include "bool.h"

struct command_t 
{
	char* command;
	char* args[512];
	char* redirect_out;
	char* redirect_in;
	bool isFirstCommand;
	bool isLastCommand;
};

int run( struct command_t* command, int input );

#endif