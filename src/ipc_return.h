#ifndef IPC_RETURN_H
#define	IPC_RETURN_H

class IPCReturn {
public:
	IPCReturn();
	IPCReturn(void* pointer, const unsigned char type);
	IPCReturn(const IPCReturn& orig);
	virtual ~IPCReturn();
	
	void SetReturn(void* pointer, const unsigned char type);
	bool IsVoid();
	
	void*               GetReturnPointer();
	const unsigned char GetReturnType();
private:
	void*               returnPointer;
	const unsigned char returnType;
};

#endif	/* IPC_RETURN_H */

