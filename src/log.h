#ifndef IP_LOG_H
#define IP_LOG_H

#include <iostream>;
void init_debugger();

void trace(const char* message);
void debug(const char* message);
void info(const char* message);
void warn(const char* message);
void error(const char* message);
void fatal(const char* message);

extern std::ostream& log;
extern std::ostream& err;

#endif
