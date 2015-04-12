#include "ipc_event.h"

#include <string>
#include <cstring>
#include <typeinfo>
#include <netinet/in.h>

#include "globals.h"
#include "logger.h"

IPCEvent::IPCEvent(char* eventName)
{
	this->compiled = false;
	this->packet   = NULL;

	this->eventName = eventName;
	this->argv.reserve(5);
	this->argt.reserve(5);
}

IPCEvent::~IPCEvent()
{
	// Free up memory
	unsigned int args = this->argv.size();
	for(unsigned int i = 0; i < args; i++)
	{
		if(this->argt[i] == IPCTypes::ch)
			delete [] (char*) this->argv[i];
	}
	
	//delete [] this->eventName;
	delete [] this->packet;
}

/*===============================================================*\
 * 
\*===============================================================*/

void IPCEvent::AddArgument(void* arg, unsigned int type)
{
	void* argv = arg;
	
	// Auto copy
	if(type == IPCTypes::ch)
	{
		argv = malloc(strlen((char*) arg) + 1);
		strcpy((char*) argv, (char*) arg);
	}
	
	this->argv.push_back(argv);
	this->argt.push_back(type);
}
	
void IPCEvent::Compile()
{
	std::lock_guard<std::mutex> lock(this->compileLock);
	
	// Clean up memory if exists
	if(this->packet != NULL)
		delete this->packet;

	unsigned int payloadSize = 0;
	unsigned int args        = this->argv.size();

	// We first estimate how total length of the payload
	payloadSize += 4;                       // Command size
	payloadSize += strlen(this->eventName); // Command
	payloadSize += 4;                       // Arg count

	for(unsigned int i = 0; i < args; i++)
	{
		// Make space for arg type
		payloadSize += 1;
		
		// We check the type then cast it if needed and add to the
		// payload size
		if(this->argt.at(i) == IPCTypes::uint || this->argt.at(i) == IPCTypes::sint)
		{
			payloadSize += 4;
		}
		else if(this->argt.at(i) == IPCTypes::ch)
		{
			// Make space for both the size and value
			payloadSize += 4;
			payloadSize += strlen((char*) this->argv.at(i));
		}
	}
	
	this->packetLen = 1 + payloadSize;

	// Prepare the packet
	uint32_t cursor = 0;
	this->packet = new char[this->packetLen];
	// --- Packet type
	memset(this->packet, 'E', 1);
	cursor += 1;
	
	// --- Command size
	unsigned int sz = strlen(this->eventName);
	uint32_t s = htonl(sz);
	memcpy(this->packet + cursor, &s, 4);
	cursor += 4;
	// --- Command
	memcpy(this->packet + cursor, this->eventName, sz);
	cursor += sz;
	// --- Args (Arg size/count)
	s = htonl(args);
	memcpy(this->packet + cursor, &s, 4);
	cursor += 4;

	// --- Argv
	for(unsigned int i = 0; i < args; i++)
	{
		// ---- Arg type
		this->packet[cursor] = this->argt.at(i);
		cursor += 1;

		// ---- Arg value
		// Depending on the arg type, we have to copy it differently
		if(this->argt.at(i) == IPCTypes::uint || this->argt.at(i) == IPCTypes::sint)
		{
			s = htonl((uint32_t) this->argv.at(i));
			memcpy(this->packet + cursor, &s, 4);
			cursor += 4;
		}
		else if(this->argt.at(i) == IPCTypes::ch)
		{
			// We have to write the size of the string and the string itself
			unsigned int argvSize = strlen((char*) this->argv.at(i));
			
			s = htonl(argvSize);
			memcpy(this->packet + cursor, &s, 4);
			cursor += 4;
			
			memcpy(this->packet + cursor, (char*) this->argv.at(i), argvSize);
			cursor += argvSize;
		}
	}

	this->compiled = true;
}

/*===============================================================*\
 * GTORS & STORS
\*===============================================================*/

bool IPCEvent::IsCompiled()
{
	return this->compiled;
}

char* IPCEvent::GetPacket()
{	
	return this->packet;
}

unsigned int IPCEvent::GetPacketSize()
{
	return this->packetLen;
}

char* IPCEvent::GetName()
{
	return this->eventName;
}

void IPCEvent::Sent()
{
	std::lock_guard<std::mutex> lock(this->sentTimeLock);
	
	this->sent++;
}

unsigned int IPCEvent::SentTimes()
{
	std::lock_guard<std::mutex> lock(this->sentTimeLock);
	
	return this->sent;
}