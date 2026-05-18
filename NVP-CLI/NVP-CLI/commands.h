#ifndef commands_h
#define commands_h

// Header contents here

#endif // commands_h

void cmd_init(int argc, char* argv[]);
void cmd_install(const char* package, const char* os, const char* nvd);
void cmd_search(const char* query, const char* os, const char* nvd);
void cmd_publish(int argc, char* argv[]);
