#include "ipc_func_man.h"
#include "ipc_function.h"

#include <pinc.h>

#include <stdint.h>

using namespace std;

IPCFuncMan::IPCFuncMan()
{
	// Reserve some space for the registered IPC functions
	this->functions.reserve(5);
	this->functionNames.reserve(5);
}

IPCFuncMan::IPCFuncMan(const IPCFuncMan& orig) {}
IPCFuncMan::~IPCFuncMan() {}

/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCFuncMan::RegisterFunction(IPCReturn (*function)(vector<void*>*, vector<uint8_t>*), const char* functionName)
{
	this->functionNames.push_back(functionName);
	this->functions.push_back(function);
}

void IPCFuncMan::ExecuteIPCFunction(IPCFunction* ipcFunction)
{
	// Loop through all registered functions and determine
	// which function to execute based on the IPC packet.
	int s = this->functions.size();
	for(int i = 0; i < s; i++)
	{
		if(strcmp(this->functionNames[i], ipcFunction->functionName) == 0)
		{
			IPCReturn ipcReturn = this->functions[i](ipcFunction->GetArgs(), ipcFunction->GetArgTypes());
			
			
		}
	}
}

/*===============================================================*\
 * IPC CALLABLE FUNCTIONS
\*===============================================================*/

IPCReturn* IPCFuncMan::GetMaxSlots(std::vector<void*>* argv, std::vector<uint8_t>* args)
{
	IPCReturn* ipcReturn = new IPCReturn();
	
	uint32_t s = Plugin_GetSlotCount();
	memcpy(ipcReturn->returnPointer, &s, 4);
	
	ipcReturn->returnType = IPCTypes::uint;
	ipcReturn->returnVoid = false;
	
	return ipcReturn;
}