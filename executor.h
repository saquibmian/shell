#ifndef executor_h
#define executor_h

#include "command.h"

// Executes the given command in the current context. 
// Handles the chaining of commands via pre-existing pipes and IO redirection. 
void execute_command( struct command_t* command, int* pipes, int input_fd );

#endif
