#include "ipc_func_man.h"
#include "ipc_function.h"
#include "ipc_return.h"
#include "logger.h"

#include <pinc.h>

#include <stdint.h>
#include <stdlib.h>

using namespace std;

IPCFuncMan::IPCFuncMan()
{
	// Reserve some space for the registered IPC functions
	this->functions.reserve(5);
	this->functionNames.reserve(5);
	
	// Register the functions
	this->RegisterFunction(IPCFuncMan::GetMaxSlots,      "GETSLOTCOUNT");
	this->RegisterFunction(IPCFuncMan::GetAllPlayerData, "PLAYERDATA");
	this->RegisterFunction(IPCFuncMan::ChatPrintf,       "CHATPRINTF");
	this->RegisterFunction(IPCFuncMan::BcastPrintf,      "BCASTPRINTF");
}

IPCFuncMan::IPCFuncMan(const IPCFuncMan& orig) {}
IPCFuncMan::~IPCFuncMan() {}

/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCFuncMan::RegisterFunction(IPCReturn* (*function)(vector<void*>*, vector<uint8_t>*), const char* functionName)
{
	this->functionNames.push_back(functionName);
	this->functions.push_back(function);
}

bool IPCFuncMan::ExecuteIPCFunction(IPCFunction* ipcFunction)
{
	bool found = false;
	
	// Loop through all registered functions and determine
	// which function to execute based on the IPC packet.
	int s = this->functions.size();
	for(int i = 0; i < s; i++)
	{
		if(strcmp(this->functionNames[i], ipcFunction->functionName) == 0)
		{
			// Execute the registered function
			IPCReturn* ipcReturn = this->functions[i](ipcFunction->GetArgs(), ipcFunction->GetArgTypes());
						
			ipcFunction->returnPointer = ipcReturn->GetReturnPointer();
			ipcFunction->returnType    = ipcReturn->GetReturnType();
			ipcFunction->returnVoid    = ipcReturn->IsVoid();
			
			// Destroy the return function, at this point we rely on the IPCFunction
			// to destroy any persistent memory that was made while creating an IPCReturn.
			delete ipcReturn;
			
			found = true;
			break;
		}
	}
	
	return found;
}

/*===============================================================*\
 * IPC CALLABLE FUNCTIONS
\*===============================================================*/

IPCReturn* IPCFuncMan::GetMaxSlots(std::vector<void*>* argv, std::vector<uint8_t>* argt)
{
	uint32_t s = Plugin_GetSlotCount();
	
	// Create a pointer to an int
	void* ipcReturnPointer = malloc(4);
	memcpy(ipcReturnPointer, &s, 4);
	
	return new IPCReturn(ipcReturnPointer, IPCTypes::uint);
}

IPCReturn* IPCFuncMan::GetAllPlayerData(vector<void*>* argv, vector<uint8_t>* argt)
{
	std::stringstream allPlayerData;
	uint32_t s = Plugin_GetSlotCount();

	for(unsigned int i = 0; i < s; i++)
	{		
		client_t* clientData = Plugin_GetClientForClientNum(i);
		netchan_t netChan    = clientData->netchan;

		// Collate the user's data into a string
		if(clientData->state > CS_FREE)
		{
			const char* ipAddr = Plugin_NET_AdrToStringShort(&netChan.remoteAddress);

			allPlayerData << i << "\\\\" << ipAddr << "\\\\";
			allPlayerData << Plugin_GetPlayerGUID(i) << "\\\\" << Plugin_GetPlayerName(i);

			allPlayerData << "\n";
		}
	}

	allPlayerData << '\0';
	
	// Pull the string out of the string stream and copy it into a persistent
	// pointer to pass on to the return.
	unsigned int size = allPlayerData.tellp();
	char* ipcReturnPointer = new char[size];
	memcpy(ipcReturnPointer, allPlayerData.str().c_str(), size);
	
	return new IPCReturn(ipcReturnPointer, IPCTypes::ch);
}

IPCReturn* IPCFuncMan::ChatPrintf(std::vector<void*>* argv, std::vector<uint8_t>* argt)
{
	// Assert 2 arguments total
	if(argv->size() != 2)
		return NULL;

	Plugin_ChatPrintf((int) argv->at(0), (char*) argv->at(1));
	
	return new IPCReturn();
}

IPCReturn* IPCFuncMan::BcastPrintf(std::vector<void*>* argv, std::vector<uint8_t>* argt)
{
	// Assert 1 argument total
	if(argv->size() != 1)
		return NULL;

	Plugin_ChatPrintf(-1, (char*) argv->at(0));
	
	return new IPCReturn();
}