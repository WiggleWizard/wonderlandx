#ifndef IPC_RETURN_H
#define	IPC_RETURN_H

class IPCReturn {
public:
	IPCReturn();
	IPCReturn(void* pointer, unsigned char type);
	IPCReturn(const IPCReturn& orig);
	virtual ~IPCReturn();
	
	void SetReturn(void* pointer, unsigned char type);
	bool IsVoid();
	
	void*         GetReturnPointer();
	unsigned char GetReturnType();
private:
	void*               returnPointer;
	unsigned char returnType;
};

#endif	/* IPC_RETURN_H */

