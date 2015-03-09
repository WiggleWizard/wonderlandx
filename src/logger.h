/* 
 * File:   logger.h
 * Author: zinglish
 *
 * Created on 08 March 2015, 17:21
 */

#ifndef LOGGER_H
#define	LOGGER_H

class Logger {
public:
	Logger();
	Logger(const Logger& orig);
	virtual ~Logger();
	
	static void LogInfo(const char* format, ...);
private:

};

#endif	/* LOGGER_H */

