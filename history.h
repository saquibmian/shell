#ifndef history_h
#define history_h

void init_history();

char* create_history_item( char* command );

void add_history_item( char* history_item );

void print_history( int count );

void cleanup_history();

#endif
