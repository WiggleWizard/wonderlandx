#include "ipc_allocator.h"

#include <unistd.h>

#include <pinc.h>

#include "logger.h"

IPCAllocator::IPCAllocator()
{
	Plugin_Printf("Chan started\n");
	
	// Create the allocator thread
	pthread_create(&this->allocatorThread, NULL, &IPCAllocator::ThreadedAllocator, this);
}

IPCAllocator::IPCAllocator(const IPCAllocator& orig) {}
IPCAllocator::~IPCAllocator() {}

/*===============================================================*\
 * THREADS
\*===============================================================*/

void* IPCAllocator::ThreadedAllocator(void* allocatorPtr)
{
	IPCAllocator* allocator = (IPCAllocator*)allocatorPtr;
	
	Logger::LogInfo("Allocator thread started");
}