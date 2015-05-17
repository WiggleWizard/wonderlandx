#include "limbo_man.h"
#include "logger.h"
#include "pinc.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

LimboMan* LimboMan::instance = NULL;

LimboMan* LimboMan::Instance()
{
	if(!instance)
		instance = new LimboMan;
	
	return instance;
}

void LimboMan::SetLimboLimit(uint8_t sz)
{
	this->wait.resize(sz, true);
	this->denied.resize(sz, false);
	this->denyReason.resize(sz, NULL);
}

void LimboMan::Reset(uint8_t slotID)
{
	this->wait.at(slotID)   = true;
	this->denied.at(slotID) = false;
}

void LimboMan::Accept(uint8_t slotID)
{
	this->wait.at(slotID)   = false;
	this->denied.at(slotID) = false;
}

void LimboMan::Deny(uint8_t slotID, char* reason)
{
	this->wait.at(slotID)   = false;
	this->denied.at(slotID) = true;
	
	// If we already have a reason, we have to clean up the memory
	// it takes up before writing a new one.
	if(this->denyReason.at(slotID))
	{
		free(this->denyReason[slotID]);
		this->denyReason[slotID] = NULL;
	}
	
	// Copy the reason to a new char* because the char* that's passed
	// in here may get deleted.
	int reasonLen = strnlen(reason, MAX_STRING_CHARS);
	this->denyReason[slotID] = (char*) malloc(reasonLen + 1);
	strcpy(this->denyReason[slotID], reason);
}

bool LimboMan::IsWaiting(uint8_t slotID)
{
	return this->wait.at(slotID);
}

bool LimboMan::IsDenied(uint8_t slotID)
{
	return this->denied.at(slotID);
}

char* LimboMan::GetDenyReason(uint8_t slotID)
{
	return this->denyReason.at(slotID);
}