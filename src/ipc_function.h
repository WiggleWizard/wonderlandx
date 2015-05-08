/**********************************************************************
 * 
 * DOCUMENTATION
 * 
 * 
 * Packet Structure
 * ----------------
 * Packet structure of a return function consists of (in order):
 *		- Packet type
 *		- Client ID
 *		- Packet ID
 *
 * ZeroMQ
 * ------
 * ZeroMQ helps to facilitate a safer transaction between client and
 * server, as well as handling packet integrity. As such we have no
 * need to add a packet length as ZeroMQ understands the length
 * and nature of the packet. We do however run into a catch 22; ZeroMQ
 * is required on both client and server side, however ZeroMQ can be
 * embedded into many languages so this should not prove to be a huge enough
 * issue to not use it.
 * 
 **********************************************************************/

#ifndef IPC_RETURN_FUNC_H
#define	IPC_RETURN_FUNC_H

#include <vector>
#include <stdint.h>

using namespace std;

class IPCFunction {
public:
	IPCFunction();
	IPCFunction(const IPCFunction& orig);
	virtual ~IPCFunction();
	
/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/
	
	/**
	 * Parses the input packet into args and function request private vars.
	 * 
     * @param packet
	 * @param destructive If true, will consume and delete the packet.
     */
	void Parse(char* payload, bool destructive);
	
	/**
	 * Executes the function and puts the return into public variables.
	 * 
	 * Warning: Deprecated
     */
	void Execute();
	
	/**
	 * Compiles the return into a return function packet.
     */
	void Compile();
	
/*===============================================================*\
 * GTORS & STORS
\*===============================================================*/
	
	char* GetPacket();
	uint32_t GetPacketLen();
	
	vector<void*>*   GetArgs();
	vector<uint8_t>* GetArgTypes();
	
/*===============================================================*\
 * VARIABLES
\*===============================================================*/
	
	void*   functionPtr;
	uint8_t functionType;
	char*   functionName;
	
	void*   returnPointer;
	uint8_t returnType;
	bool    returnVoid;
private:
	uint32_t clientID;
	uint32_t packetID;
	
	vector<void*>   argv;
	vector<uint8_t> argt;
	
	uint32_t packetLen;
	char*    packet;
};

#endif	/* IPC_RETURN_FUNC_H */
