#include "ipc_return_func.h"

#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <string>
#include <arpa/inet.h>

#include <pinc.h>

#include "globals.h"
#include "logger.h"

IPCReturnFunction::IPCReturnFunction()
{
	this->argv.reserve(5);
	this->argt.reserve(5);
	
	this->packet             = NULL;
	this->functionReturnPtr  = NULL;
	this->functionReturnType = 0;
}

IPCReturnFunction::IPCReturnFunction(const IPCReturnFunction& orig) {}
IPCReturnFunction::~IPCReturnFunction()
{
	// Free up arg memory
	unsigned int args = this->argv.size();
	for(unsigned int i = 0; i < args; i++)
	{
		if(this->argt[i] == IPCTypes::uint || this->argt[i] == IPCTypes::sint)
			delete (uint32_t*) this->argv[i];
		else if(this->argt[i] == IPCTypes::ch)
			delete [] (char*) this->argv[i];
			
		this->argv[i] = NULL;
	}
	
	delete [] this->functionName;
	this->functionName = NULL;
	
	// Decide how to delete the return based on its stored type
	if(this->functionReturnType == IPCTypes::uint || this->functionReturnType == IPCTypes::sint)
		delete (uint32_t*) this->functionReturnPtr;
	else if(this->functionReturnType == IPCTypes::ch)
		delete [] (char*) this->functionReturnPtr;
	this->functionReturnPtr = NULL;
		
	delete [] this->packet;
	this->packet = NULL;
}

/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCReturnFunction::Parse(char* payload, bool destructive)
{
	unsigned int cursor = 0;
	
	// - Packet type
	cursor += 1;
	
	// - Client ID
	this->clientID = ntohl(*(uint32_t*) (payload + cursor));
	cursor += 4;
	
	// - Packet ID
	this->packetID = ntohl(*(uint32_t*) (payload + cursor));
	cursor += 4;
	
	// - Function name size
	uint32_t funcSize = ntohl(*(uint32_t*) (payload + cursor));
	cursor += 4;
	
	// - Function name
	this->functionName = new char[funcSize + 1];
	this->functionName[funcSize] = '\0';
	memcpy(this->functionName, payload + cursor, funcSize);
	cursor += funcSize;
	
	// - Arg count
	uint8_t argc = *(uint8_t*) (payload + cursor);
	cursor += 1;
	
	// - Args
	this->argv.reserve(argc);
	this->argt.reserve(argc);
	for(unsigned int i = 0; i < argc; i++)
	{
		// -- Type
		this->argt.push_back(*(uint8_t*) (payload + cursor));
		cursor += 1;
		
		// -- Value
		if(this->argt[i] == IPCTypes::uint || this->argt[i] == IPCTypes::sint)
		{
			// We first have to convert the network byte order integer to host
			// order then make space for it in memory so we can keep it for
			// later use.
			uint32_t integer = ntohl(*(uint32_t*) (payload + cursor));
			char* intSpace = new char[4];
			
			memcpy(intSpace, &integer, 4);
			this->argv.push_back(intSpace);
			
			cursor += 4;
		}
		else if(this->argt[i] == IPCTypes::ch)
		{
			// --- Size of string
			uint32_t s = ntohl(*(uint32_t*) (payload + cursor));
			cursor += 4;
			
			// --- String value
			char* str = new char[s + 1];
			memcpy(str, payload + cursor, s);
			str[s] = '\0';
			this->argv.push_back(str);
			
			cursor += s;
		}
	}
}

void IPCReturnFunction::Execute()
{
	Logger::Debug("Attempting to execute return function '%s' for client %i", this->functionName, this->clientID);
	
	if(strcmp(this->functionName, "GETSLOTCOUNT") == 0)
	{
		this->functionReturnPtr = new char[4];
		
		uint32_t s = Plugin_GetSlotCount();
		memcpy(this->functionReturnPtr, &s, 4);
		
		this->functionReturnType = IPCTypes::uint;
	}
}

void IPCReturnFunction::Compile()
{
	// Clean up memory if exists
	if(this->packet != NULL)
	{
		delete [] this->packet;
		this->packet = NULL;
	}
	
	// Calculate packet length
	this->packetLen = 1;  // Packet type
	this->packetLen += 4; // Client ID
	this->packetLen += 4; // Packet ID
	this->packetLen += 1; // Return type
	
	if(this->functionReturnType == IPCTypes::uint || this->functionReturnType == IPCTypes::sint)
		this->packetLen += 4;
	else if(this->functionReturnType == IPCTypes::ch)
	{
		this->packetLen += 4;
		this->packetLen += strlen((char*) this->functionReturnPtr);
	}
	
	// Construct a new packet
	this->packet = new char[this->packetLen + 1];
	
	// Compile the actual data into the packet
	u_int32_t cursor = 0;
	
	// - Packet type
	this->packet[0] = 'R';
	cursor += 1;
	// - Client ID
	u_int32_t s = htonl(this->clientID);
	memcpy(this->packet + cursor, &s, 4);
	cursor += 4;
	// - Packet ID
	s = htonl(this->packetID);
	memcpy(this->packet + cursor, &s, 4);
	cursor += 4;
	
	// - Return type
	memcpy(this->packet + cursor, &this->functionReturnType, 1);
	cursor += 1;
	
	// - Return value
	if(this->functionReturnType == IPCTypes::uint || this->functionReturnType == IPCTypes::sint)
	{
		s = htonl(*(uint32_t*) this->functionReturnPtr);
		memcpy(this->packet + cursor, &s, 4);
	}
	else if(this->functionReturnType == IPCTypes::ch)
	{
		s = htonl(strlen((char*) this->functionReturnPtr));
		memcpy(this->packet + cursor, &s, 4);
		cursor += 4;
		
		memcpy(this->packet + cursor, (char*) this->functionReturnPtr, strlen((char*) this->functionReturnPtr));
	}
}

/*===============================================================*\
 * GTORS & STORS
\*===============================================================*/

char* IPCReturnFunction::GetPacket()
{
	return this->packet;
}

uint32_t IPCReturnFunction::GetPacketLen()
{
	return this->packetLen;
}