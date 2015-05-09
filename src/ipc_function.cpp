#include "ipc_function.h"

#include <netinet/in.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>

#include <pinc.h>

#include "globals.h"
#include "logger.h"

IPCFunction::IPCFunction()
{
	this->argv.reserve(5);
	this->argt.reserve(5);
	
	this->packet       = NULL;
	this->functionPtr  = NULL;
	this->functionType = 0;
}

IPCFunction::IPCFunction(const IPCFunction& orig) {}
IPCFunction::~IPCFunction()
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
	if(this->functionType == IPCTypes::uint || this->functionType == IPCTypes::sint)
		delete (uint32_t*) this->functionPtr;
	else if(this->functionType == IPCTypes::ch)
		delete [] (char*) this->functionPtr;
	this->functionPtr = NULL;
		
	delete [] this->packet;
	this->packet = NULL;
	
	if(this->returnType == IPCTypes::uint || this->returnType == IPCTypes::sint)
		delete (uint32_t*) this->returnPointer;
	else if(this->returnType == IPCTypes::ch)
		delete [] (char*) this->returnPointer;
	this->returnPointer = NULL;
}



/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCFunction::Parse(char* payload, bool destructive)
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

void IPCFunction::Compile()
{
	// Clean up memory if exists
	if(this->packet != NULL)
	{
		delete [] this->packet;
		this->packet = NULL;
	}
	
	// Calculate packet length
	this->packetLen  = 1;  // Packet type
	this->packetLen += 4; // Client ID
	this->packetLen += 4; // Packet ID
	this->packetLen += 1; // Return type
	
	if(!this->returnVoid)
	{
		if(this->returnType == IPCTypes::uint || this->returnType == IPCTypes::sint)
			this->packetLen += 4;
		else if(this->returnType == IPCTypes::ch)
		{
			this->packetLen += 4;
			this->packetLen += strlen((char*) this->functionPtr);
		}
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
	
	// If there is no return then don't put it into the packet.
	if(!this->returnVoid)
	{
		memcpy(this->packet + cursor, &this->returnType, 1);
		cursor += 1;

		// - Return value
		if(this->returnType == IPCTypes::uint || this->returnType == IPCTypes::sint)
		{
			s = htonl(*(uint32_t*) this->returnPointer);
			memcpy(this->packet + cursor, &s, 4);
		}
		else if(this->returnType == IPCTypes::ch)
		{
			s = htonl(strlen((char*) this->returnPointer));
			memcpy(this->packet + cursor, &s, 4);
			cursor += 4;

			memcpy(this->packet + cursor, (char*) this->returnPointer, strlen((char*) this->returnPointer));
		}
	}
}

/*===============================================================*\
 * GTORS & STORS
\*===============================================================*/

char* IPCFunction::GetPacket()
{
	return this->packet;
}

uint32_t IPCFunction::GetPacketLen()
{
	return this->packetLen;
}

std::vector<void*>* IPCFunction::GetArgs()
{
	return &this->argv;
}

std::vector<uint8_t>* IPCFunction::GetArgTypes()
{
	return &this->argt;
}