#ifndef parser_h
#define parser_h

#include "bool.h"
#include "command.h"

// Parses a command, generating its arguments list 
// and storing any IO redirection parameters as well
void parse_command( struct command_t* command );

#endif