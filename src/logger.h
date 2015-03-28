#ifndef LOGGER_H
#define	LOGGER_H

class Logger {
public:
	Logger();
	Logger(const Logger& orig);
	virtual ~Logger();
	
	static void LogInfo(const char* format, ...);
	static void Debug(const char* format, ...);
private:

};

#endif	/* LOGGER_H */

