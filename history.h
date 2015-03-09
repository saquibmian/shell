#ifndef history_h
#define history_h

// Initialize the history data storage
void init_history();

// Create an immutable copy of the command
char* create_history_item( char* command );

// Add a command ot the history
void add_history_item( char* history_item );

// Print the last 'count' history commands
void print_history( int count );

// Release the history data storage
void cleanup_history();

#endif
