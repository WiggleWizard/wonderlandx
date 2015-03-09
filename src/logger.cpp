#include "logger.h"

#include <cstdarg>

#include <pinc.h>

Logger::Logger() {}
Logger::Logger(const Logger& orig) {}
Logger::~Logger() {}

void Logger::LogInfo(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	
	Plugin_Printf("[WonderlandX][\033[1;34mInfo\033[0m]: ");
	Plugin_Printf(format, args);
	Plugin_Printf("\n");
	
	va_end(args);
}