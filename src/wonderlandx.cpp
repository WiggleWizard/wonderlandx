#include <stdio.h>
#include <unistd.h>
#include <pinc.h>

#include "globals.h"
#include "ipc_rabbithole.h"
#include "ipc_event.h"
#include "logger.h"

IPCRabbithole* rabbithole;

PCL int OnInit()
{
	Plugin_Printf("Initialising WonderlandX\n");
	
	int serverPort = Plugin_Cvar_VariableIntegerValue("net_port");
	
	rabbithole = new IPCRabbithole(serverPort);
	
	return 0;
}

PCL void OnInfoRequest(pluginInfo_t *info)
{
	// =====  MANDATORY FIELDS  =====
	info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
	info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;  // Requested handler version

	// =====  OPTIONAL  FIELDS  =====
	info->pluginVersion.major = PLUGIN_VERSION_MAJOR;
	info->pluginVersion.minor = PLUGIN_VERSION_MINOR;
	strncpy(info->fullName,"WonderlandX", sizeof(info->fullName));
	strncpy(info->shortDescription, "Wonderland for CoD4X17a", sizeof(info->shortDescription));
}

PCL void OnPlayerConnect(int clientnum, netadr_t* netaddress, char* pbguid, char* userinfo, int authstatus, char* deniedmsg, int deniedmsgbufmaxlen)
{
	// Persist the user information
	char* userInfo = (char*) malloc(strlen(userinfo));
	strncpy(userInfo, userinfo, strlen(userinfo));
	
	// Byte array to little endian
	uint ipAddr = 
			(netaddress->ip[3] << 24) |
			(netaddress->ip[2] << 16) |
			(netaddress->ip[1] << 8)  |
			(netaddress->ip[0]);
	
	IPCCoD4Event* event = new IPCCoD4Event("PLAYERJOIN");
	event->AddArgument((void*) clientnum, IPCTypes::sint);
	event->AddArgument((void*) ipAddr, IPCTypes::sint);
	event->AddArgument((void*) userInfo, IPCTypes::ch);
	
	rabbithole->SetEventForBroadcast(event);
	
	rabbithole->SignalEventSend();
}