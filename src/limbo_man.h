#ifndef LIMBO_H
#define	LIMBO_H

#include <stdint.h>
#include <vector>

class LimboMan {
public:
	static LimboMan* Instance();
	
	void SetLimboLimit(uint8_t sz);
	
	void Reset(uint8_t slotID);
	void Accept(uint8_t slotID);
	void Deny(uint8_t slotID, char* reason);
	
	bool IsWaiting(uint8_t slotID);
	bool IsDenied(uint8_t slotID);
	char* GetDenyReason(uint8_t slotID);
	
private:
	LimboMan(){};
	LimboMan(const LimboMan& orig){};
	virtual ~LimboMan(){};
	
	static LimboMan* instance;
	
	std::vector<bool> wait;
	std::vector<bool> denied;
	std::vector<char*> denyReason;
};

#endif	/* LIMBO_H */

