#include "ipc_rabbithole.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <pinc.h>

#include "logger.h"

IPCRabbithole::IPCRabbithole(int serverPort)
{
	// Reserve some slots for event sending
	this->queueEvents.reserve(8);
	uint s = this->queueEvents.size();
	for(uint i = 0; i < s; i++)
		this->queueEvents[i] = NULL;
	
	this->serverPort = serverPort;
	this->ipcBasePath = "/tmp/rabbithole.ipc\0";
	
	// Build the path to the IPC socket
	int strLen = strlen(this->ipcBasePath) + 6 + 1;
	this->ipcFullPath = (char*) malloc(strLen);
	snprintf(this->ipcFullPath, strLen, "%s.%d",this->ipcBasePath, this->serverPort);
	
	// Create the allocator thread
	pthread_create(&this->rabbitholeThread, NULL, &IPCRabbithole::ThreadedRabbitholeInit, this);
}

IPCRabbithole::IPCRabbithole(const IPCRabbithole& orig) {}
IPCRabbithole::~IPCRabbithole() {}

/*===============================================================*\
 * THREADS
\*===============================================================*/

void* IPCRabbithole::ThreadedRabbitholeInit(void* rabbitholePtr)
{
	IPCRabbithole* rabbithole = (IPCRabbithole*)rabbitholePtr;
	
	Logger::LogInfo("Rabbit hole started, attempting to bind IPC @ %s", rabbithole->ipcFullPath);
	
	rabbithole->zmqContext = zmq_ctx_new();
	rabbithole->zmqSocket  = zmq_socket(rabbithole->zmqContext, ZMQ_DEALER);
	
	// Build the bind protocol
	int strLen = strlen(rabbithole->ipcFullPath) + 6 + 1;
	rabbithole->ipcProtocol = (char*) malloc(strLen);
	snprintf(rabbithole->ipcProtocol, strLen, "ipc://%s", rabbithole->ipcFullPath);
	
	int rc = zmq_bind(rabbithole->zmqSocket, rabbithole->ipcProtocol);
	
	if(!rc)
		Logger::LogInfo("IPC bind successful");
	else
	{
		Logger::LogInfo("IPC bind unsuccessful");
		exit(0);
	}
	
	// Thread out the event sender
	pthread_create(&rabbithole->rabbitholeThread, NULL, &IPCRabbithole::ThreadedEventSender, rabbithole);
	
	/*while(1)
	{
		zmq_msg_t msg;
		zmq_msg_init(&msg);
	
		int size = zmq_recvmsg(rabbithole->zmqSocket, &msg, 0);

		Logger::LogInfo("MSG [size-> %i]: %s", zmq_msg_size(&msg), (char*)zmq_msg_data(&msg));
		
		zmq_msg_close(&msg);
	}*/
}

void* IPCRabbithole::ThreadedEventSender(void* rabbitholePtr)
{
	IPCRabbithole* rabbithole = (IPCRabbithole*) rabbitholePtr;
	
	while(true)
	{
		// Create a scope lock and wait for signal
		std::unique_lock<std::mutex> unqlockSendEvent(rabbithole->lockSendEvent);
		rabbithole->condSendEvent.wait(unqlockSendEvent);
		
		// Compile then broadcast any events in the queue
		uint s = rabbithole->queueEvents.size();
		for(uint i = 0; i < s; i++)
		{
			IPCCoD4Event* event = rabbithole->queueEvents.at(i);
			
			if(event != NULL)
			{
				event->Compile();
				
				zmq_msg_t msg;
				zmq_msg_init_data(&msg, event->GetPacket(), event->GetPacketSize(), NULL, NULL);
				
				zmq_sendmsg(rabbithole->zmqSocket, &msg, ZMQ_DONTWAIT);
				
				zmq_msg_close(&msg);
				
				rabbithole->DestroyEvent(event);
			}
		}
		
		unqlockSendEvent.unlock();
	}
}


/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCRabbithole::SetEventForBroadcast(IPCCoD4Event* event)
{
	std::unique_lock<std::mutex> lck(this->lockQueueEvents);
	
	// Find a free slot to push the event into, this avoids
	// stack re-allocation when we simply just push into the
	// vector.
	bool found = false;
	int s = this->queueEvents.size();
	for(int i = 0; i < s; i++)
	{
		if(this->queueEvents.at(i) == NULL)
		{
			this->queueEvents[i] = event;
			found = true;
			
			break;
		}
	}
	
	// If a free slot is not found then we just force the queue
	// larger and put it at the bottom.
	if(!found)
		this->queueEvents.push_back(event);
	
	lck.unlock();
	
	// Signal that an event has been put into the queue
	this->SignalEventSend();
}

void IPCRabbithole::DestroyEvent(IPCCoD4Event* event)
{
	unsigned int s = this->queueEvents.size();
	for(unsigned int i = 0; i < s; i++)
	{
		if(event == this->queueEvents.at(i))
		{
			// Free the memory first
			delete this->queueEvents[i];

			this->queueEvents[i] = NULL;
		}	
	}
}

void IPCRabbithole::SignalEventSend()
{
	this->condSendEvent.notify_one();
}