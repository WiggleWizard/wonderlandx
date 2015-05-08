#ifndef IPC_FUNC_MAN_H
#define	IPC_FUNC_MAN_H

#include <vector>
#include <stdint.h>

#include "globals.h"
#include "ipc_function.h"

using namespace std;

class IPCFuncMan {
public:
	IPCFuncMan();
	IPCFuncMan(const IPCFuncMan& orig);
	virtual ~IPCFuncMan();
	
/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/
	
	/**
	 * Registers a function that can be called from IPC.
	 * 
	 * @param functionName Reference is stored. Memory is managed internally.
     * @param function     Reference is stored. Memory is managed internally.
     */
	void RegisterFunction(IPCReturn (*function)(std::vector<void*>*, std::vector<uint8_t>*), const char* functionName);
	
	/**
	 * IPCFunction is modified once executed. This function does not compile
	 * after execution.
	 * 
	 * Multiple executions of an IPCFunction is fine, as long as it's OK with
	 * having multiple modifications to the object.
	 * 
     * @param ipcFunction
     */
	void ExecuteIPCFunction(IPCFunction* ipcFunction);
	
/*===============================================================*\
 * IPC CALLABLE FUNCTIONS
\*===============================================================*/
	
	static IPCReturn* GetMaxSlots(vector<void*>* argv, vector<uint8_t>* argt);
	
private:
	std::vector<IPCReturn (*)(std::vector<void*>*, std::vector<uint8_t>*)> functions;
	std::vector<const char*> functionNames;
};

#endif	/* IPC_FUNC_MAN_H */

