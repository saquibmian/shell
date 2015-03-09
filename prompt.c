#include "prompt.h"

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

static const char* username = NULL;

static const char* get_username()
{
	struct passwd* pw = getpwuid( geteuid() );
	if ( pw )
	{
		return pw->pw_name;
	}

	return "";
}

void prompt() 
{
	if ( username == NULL ) username = get_username();

	printf( "%s> ", username );
	fflush( NULL );
}