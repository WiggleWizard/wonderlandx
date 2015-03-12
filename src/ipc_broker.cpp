#include "ipc_broker.h"

#include <unistd.h>
#include <stdlib.h>

#include <pinc.h>

#include "logger.h"

IPCBroker::IPCBroker()
{
	Plugin_Printf("Chan started\n");
	
	// Create the allocator thread
	pthread_create(&this->brokerThread, NULL, &IPCBroker::ThreadedBroker, this);
}

IPCBroker::IPCBroker(const IPCBroker& orig) {}
IPCBroker::~IPCBroker() {}

/*===============================================================*\
 * THREADS
\*===============================================================*/

void* IPCBroker::ThreadedBroker(void* brokerPtr)
{
	IPCBroker* broker = (IPCBroker*)brokerPtr;
	
	Logger::LogInfo("Broker started, attempting to bind IPC");
	
	broker->zmqContext = zmq_ctx_new();
	broker->zmqSocket  = zmq_socket(broker->zmqContext, ZMQ_REP);
	
	zmq_bind(broker->zmqSocket, "ipc://broker.ipc");
	
	Logger::LogInfo("IPC bind successful");
	
	char* buffer = (char*) malloc(256);
	
	while(1)
	{
		memset(buffer, 0, 256);
		
		zmq_msg_t msg;
		zmq_msg_init(&msg);
		
		int size = zmq_recv(broker->zmqSocket, buffer, 255, 0);
		
		sleep(2);
	}
}