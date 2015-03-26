#ifndef IPC_RETURN_FUNC_H
#define	IPC_RETURN_FUNC_H

#include <vector>
#include <stdint.h>

class IPCReturnFunction {
public:
	IPCReturnFunction();
	IPCReturnFunction(const IPCReturnFunction& orig);
	virtual ~IPCReturnFunction();
	
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
	
/*===============================================================*\
 * VARIABLES
\*===============================================================*/
	
	void* functionReturnPtr;
	uint8_t functionReturnType;
	char* functionName;
private:
	uint32_t packetID;
	std::vector<void*> argv;
	std::vector<uint8_t> argt;
	
	uint32_t packetLen;
	char* packet;
};

#endif	/* IPC_RETURN_FUNC_H */
