#include "ipc_rabbithole.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <pinc.h>

#include "logger.h"
#include "ipc_function.h"
#include "ipc_return.h"

IPCRabbithole::IPCRabbithole(int serverPort)
{	
	// Build the IPC function manager
	this->ipcFuncMan = new IPCFuncMan();
	
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
	
	Logger::Debug("Rabbit hole started, attempting to bind IPC @ %s", rabbithole->ipcFullPath);
	
	rabbithole->zmqContext = zmq_ctx_new();
	rabbithole->zmqSocket  = zmq_socket(rabbithole->zmqContext, ZMQ_DEALER);
	
	// Build the bind protocol
	int strLen = strlen(rabbithole->ipcFullPath) + 6 + 1;
	rabbithole->ipcProtocol = (char*) malloc(strLen);
	snprintf(rabbithole->ipcProtocol, strLen, "ipc://%s", rabbithole->ipcFullPath);
	
	int rc = zmq_bind(rabbithole->zmqSocket, rabbithole->ipcProtocol);
	
	if(!rc)
		Logger::Debug("IPC bind successful");
	else
	{
		Logger::Debug("IPC bind unsuccessful");
		exit(0);
	}
	
	// Thread out the event sender
	pthread_create(&rabbithole->eventThread, NULL, &IPCRabbithole::ThreadedEventSender, rabbithole);
	
	// Thread out the receiver
	pthread_create(&rabbithole->rxThread, NULL, &IPCRabbithole::ThreadedRX, rabbithole);
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
			IPCEvent* event = rabbithole->queueEvents.at(i);
			
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
			Logger::Debug("Received a 0 length message, skipping");
			continue;
		}
		
		if(rxData[0] == 'F')
		{
			IPCFunction ipcFunction = IPCFunction();
			ipcFunction.Parse(rxData, false);
			
			Logger::Debug("Recieved IPC function call: %s", ipcFunction.functionName);
			
			// Attempt to execute the called function, if there is no function
			// registered then we simply move on.
			//
			// The input IPCFunction is modified to include the return ptr and
			// return type for compile time.
			int execResult = rabbithole->ipcFuncMan->ExecuteIPCFunction(&ipcFunction);
			
			if(!execResult)
			{
				Logger::Debug("IPC function (%s) not registered to execute", ipcFunction.functionName);
				continue;
			}
			
			if(ipcFunction.returnVoid)
				continue;
			
			// Compile the IPCFunction so we can send the results to the rabbithole
			ipcFunction.Compile();
			
			// We have to copy the data into a temporary memory section because
			// if we try to delete the data right after the send function we
			// end up with a data race because ZMQ hasn't finished with the message
			// by the time we manually delete the data.
			char* txData = (char*) malloc(ipcFunction.GetPacketLen());
			memcpy(txData, ipcFunction.GetPacket(), ipcFunction.GetPacketLen());
		
			// Create the ZMQ message and then broadcast it
			zmq_msg_t txMsg;
			zmq_msg_init_data(&txMsg, txData, ipcFunction.GetPacketLen(), IPCRabbithole::ZMQDestroyMsgData, NULL);
			
			zmq_sendmsg(rabbithole->zmqSocket, &txMsg, ZMQ_DONTWAIT);
			
			zmq_msg_close(&txMsg);
		}
	}
}



/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/

void IPCRabbithole::SetEventForBroadcast(IPCEvent* event)
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

void IPCRabbithole::DestroyEvent(IPCEvent* event)
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