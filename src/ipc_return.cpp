#include "ipc_return.h"
#include "globals.h"

IPCReturn::IPCReturn()
{
	this->returnType    = IPCTypes::null;
	this->returnPointer = 0x00;
}

IPCReturn::IPCReturn(void* pointer, unsigned char type)
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

unsigned char IPCReturn::GetReturnType()
{
	return this->returnType;
}

bool IPCReturn::IsVoid()
{
	return this->returnType == IPCTypes::null ? true : false;
}