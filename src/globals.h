#ifndef GLOBALS_H
#define	GLOBALS_H

#define PLUGIN_VERSION_MAJOR 0;
#define PLUGIN_VERSION_MINOR 2;

extern struct IPCTypes {
	static const unsigned char uint = 0x01; // Unsigned int
	static const unsigned char sint = 0x02; // Signed int
	static const unsigned char ch   = 0x03; // Characters
} IPCTypes;

extern bool WONDERLANDX_DBG;
extern bool WONDERLANDX_EVENT_DBG;
extern bool WONDERLANDX_RTN_FUNC_DBG;
extern bool WONDERLANDX_VOID_FUNC_DBG;

#endif	/* GLOBALS_H */