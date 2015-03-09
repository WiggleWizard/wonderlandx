#ifndef IPC_CHAN_H
#define	IPC_CHAN_H

#include <pthread.h>

#include <zmq.h>

class IPCAllocator {
public:
	IPCAllocator();
	IPCAllocator(const IPCAllocator& orig);
	virtual ~IPCAllocator();
	
/*===============================================================*\
 * THREADS
\*===============================================================*/
	
	/**
	 * Handles connection requests.
	 * 
     * @param allocatorPtr
     * @return 
     */
	static void* ThreadedAllocator(void* allocatorPtr);
	
private:
	pthread_t allocatorThread;
	
};

#endif	/* IPC_CHAN_H */