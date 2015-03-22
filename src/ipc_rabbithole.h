#ifndef IPC_CHAN_H
#define	IPC_CHAN_H

#include <pthread.h>
#include <condition_variable>
#include <vector>

#include <zmq.h>

#include "ipc_event.h"

class IPCRabbithole {
public:
	IPCRabbithole(int serverPort);
	IPCRabbithole(const IPCRabbithole& orig);
	virtual ~IPCRabbithole();
	
/*===============================================================*\
 * THREADS
\*===============================================================*/
	
	/**
	 * Handles connection requests.
	 * 
     * @param rabbitholePtr
     * @return 
     */
	static void* ThreadedRabbitholeInit(void* rabbitholePtr);
	
	/**
	 * Asyncronously sends the triggered events to all connected
	 * IPC clients. The events are also compiled in this thread
	 * to avoid any unnecessary delay inside the CoD main thread.
	 * 
     * @param rabbitholePtr
     * @return 
     */
	static void* ThreadedEventSender(void* rabbitholePtr);
	
	
	
/*===============================================================*\
 * FUNCTIONS
\*===============================================================*/
	
	/**
	 * Sets an event for broadcasting across all connected clients.
	 * 
     * @param event
     */
	void SetEventForBroadcast(IPCCoD4Event* event);
	
	/**
	 * Removes the event from the queue and frees up the memory.
	 * 
     * @param event
     */
	void DestroyEvent(IPCCoD4Event* event);
	
	void SignalEventSend();
	
	/**
	 * Used by ZMQ to destroy msg data after it's sent.
	 * 
     * @param data
     * @param hint
     */
	static void ZMQDestroyMsgData(void* data, void* hint);
	
private:
	pthread_t rabbitholeThread;
	void*     zmqContext;
	void*     zmqSocket;
	
	int         serverPort;
	const char* ipcBasePath;
	char*       ipcFullPath;
	char*       ipcProtocol;
	
	std::condition_variable condSendEvent;
	std::mutex              lockSendEvent;
	
	std::vector<IPCCoD4Event*> queueEvents;
	std::mutex                 lockQueueEvents;
};

#endif	/* IPC_CHAN_H */