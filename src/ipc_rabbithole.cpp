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
				
				// After compile, we copy the packet across to open memory
				// so that ZMQ cleans it up after it's done and we can attend
				// to the event when ever we want.
				char* msgData = (char*) malloc(event->GetPacketSize());
				memcpy(msgData, event->GetPacket(), event->GetPacketSize());
				
				zmq_msg_t msg;
				zmq_msg_init_data(&msg, msgData, event->GetPacketSize(), IPCRabbithole::ZMQDestroyMsgData, NULL);
				
				zmq_sendmsg(rabbithole->zmqSocket, &msg, ZMQ_DONTWAIT);
				
				zmq_msg_close(&msg);
				
				rabbithole->DestroyEvent(event);
			}
		}
		
		unqlockSendEvent.unlock();
	}
}

void* IPCRabbithole::ThreadedRX(void* rabbitholePtr)
{
	IPCRabbithole* rabbithole = (IPCRabbithole*) rabbitholePtr;
	
	while(true)
	{
		zmq_msg_t rxMsg;
		zmq_msg_init(&rxMsg);
	
		zmq_recvmsg(rabbithole->zmqSocket, &rxMsg, 0);
		char* rxData = (char*) zmq_msg_data(&rxMsg);
		
		if(zmq_msg_size(&rxMsg) < 1)
		{
			Logger::LogInfo("Received a 0 length message, skipping");
			continue;
		}
		
		// Void function
		if(rxData[0] == 'V')
		{
			
		}
		// Return function
		else if(rxData[0] == 'R')
		{
			IPCReturnFunction returnFunc = IPCReturnFunction();
			returnFunc.Parse(rxData, false);
			
			// Now we execute the function that was sent and we return any results
			// of the executed function.
			returnFunc.Execute();
			returnFunc.Compile();
			
			// We have to copy the data into a temporary memory section in order to
			// handle data racing.
			char* txData = (char*) malloc(returnFunc.GetPacketLen());
			memcpy(txData, returnFunc.GetPacket(), returnFunc.GetPacketLen());
		
			// Create the ZMQ message and then broadcast it
			zmq_msg_t txMsg;
			zmq_msg_init_data(&txMsg, txData, returnFunc.GetPacketLen(), IPCRabbithole::ZMQDestroyMsgData, NULL);
			
			zmq_sendmsg(rabbithole->zmqSocket, &txMsg, ZMQ_DONTWAIT);
			
			zmq_msg_close(&txMsg);
		}
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

void IPCRabbithole::ZMQDestroyMsgData(void* data, void* hint)
{
	free(data);
}