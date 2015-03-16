#ifndef IPC_EVENT_PKT_H
#define	IPC_EVENT_PKT_H

#include <string>
#include <typeinfo>
#include <vector>
#include <mutex>

#include "globals.h"

class IPCCoD4Event {
public:
	IPCCoD4Event(char* eventName);

	IPCCoD4Event(const IPCCoD4Event& orig);
	virtual ~IPCCoD4Event();
	
/*===============================================================*\
 * 
\*===============================================================*/
	
	void Compile();
	
	/**
	 * @Arguments
	 * - Strings need to be char pointers that persist in memory: `char* s = new char[9];`.
	 * s is then passed into AddArgument while cast to a void*.
	 * 
	 * - Integers are passed into the function by value (Cast into void*): 
	 * AddArgument((void*) 10, 0x01 -OR- 0x02).
	 * 
     * @param arg 
     * @param type Type can be: 0x01 (uint32_t) / 0x02 (int32_t) / 0x03 (char*)
     */
	void AddArgument(void* arg, unsigned int type);

/*===============================================================*\
 * GTORS & STORS
\*===============================================================*/

	bool IsCompiled();
	char* GetPacket();
	unsigned int GetPacketSize();
	char* GetName();
	void Sent();
	unsigned int SentTimes();

private:
	char* eventName;
	std::vector<void*> argv;
	std::vector<unsigned int> argt;

	bool compiled;
	unsigned int sent;
	char* packet;
	unsigned int packetLen;

	std::mutex compileLock;
	std::mutex sentTimeLock;
};

#endif	/* IPC_EVENT_PKT_H */

