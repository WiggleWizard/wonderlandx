#include "ipc_return.h"
#include "globals.h"

IPCReturn::IPCReturn()
{
	this->returnType    = IPCTypes::null;
}

IPCReturn::IPCReturn(void* pointer, const unsigned char type)
{
	this->returnPointer = pointer;
	this->returnType    = type;
}

IPCReturn::IPCReturn(const IPCReturn& orig) {}
IPCReturn::~IPCReturn() {}

void* IPCReturn::GetReturnPointer()
{
	return this->returnPointer;
}

const unsigned char* IPCReturn::GetReturnPointer()
{
	return this->returnType;
}

bool IPCReturn::IsVoid()
{
	return this->returnType == IPCTypes::null ? false : true;
}