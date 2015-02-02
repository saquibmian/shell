#ifndef executor_h
#define executor_h

#include "command.h"

void execute_command( struct command_t* command, int* pipes, int input_fd );

#endif
