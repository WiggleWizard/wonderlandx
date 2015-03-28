/* 
 * File:   globals.h
 * Author: zinglish
 *
 * Created on 07 March 2015, 19:51
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#define PLUGIN_VERSION_MAJOR 0;
#define PLUGIN_VERSION_MINOR 2;

extern struct IPCTypes {
	static const unsigned char uint = 0x01; // Unsigned int
	static const unsigned char sint = 0x02; // Signed int
	static const unsigned char ch   = 0x03; // Characters
} IPCTypes;

extern bool DEBUG_MODE = 0;

#endif	/* GLOBALS_H */

