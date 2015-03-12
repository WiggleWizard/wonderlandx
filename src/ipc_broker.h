#ifndef IPC_CHAN_H
#define	IPC_CHAN_H

#include <pthread.h>

#include <zmq.h>

class IPCBroker {
public:
	IPCBroker();
	IPCBroker(const IPCBroker& orig);
	virtual ~IPCBroker();
	
/*===============================================================*\
 * THREADS
\*===============================================================*/
	
	/**
	 * Handles connection requests.
	 * 
     * @param allocatorPtr
     * @return 
     */
	static void* ThreadedBroker(void* allocatorPtr);
	
	static char* s_recv(void* socket);
	
private:
	pthread_t brokerThread;
	void* zmqContext;
	void* zmqSocket;
};

#endif	/* IPC_CHAN_H */