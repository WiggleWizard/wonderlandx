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
	// Plugin arguments
	uint s = Plugin_Cmd_Argc();
	for(uint i = 0; i < s; i++)
	{
		char* argv = Plugin_Cmd_Argv(i);
		
		if(strcmp(argv, "-d") == 0)
		{
			WONDERLANDX_DBG = true;
			
			Logger::LogInfo("Debug mode enabled");
		}
	}
	
	Logger::LogInfo("Initialising WonderlandX\n");
	
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
	// Byte array to little endian
	uint ipAddr = 
			(netaddress->ip[3] << 24) |
			(netaddress->ip[2] << 16) |
			(netaddress->ip[1] << 8)  |
			(netaddress->ip[0]);
	
	IPCEvent* event = new IPCEvent("JOIN");
	event->AddArgument((void*) clientnum, IPCTypes::uint);
	event->AddArgument((void*) ipAddr, IPCTypes::uint);
	event->AddArgument((void*) Plugin_GetPlayerGUID(clientnum), IPCTypes::ch);
	event->AddArgument((void*) userinfo, IPCTypes::ch);
	
	rabbithole->SetEventForBroadcast(event);
	
	rabbithole->SignalEventSend();
}

PCL void OnMessageSent(char* message, int slot, qboolean *show, int mode)
{
	// Force the server not to forward the message on to the CoD4 clients
	// because Alice will deal with them.
	*show = qfalse;
	
	IPCEvent* event = new IPCEvent("CHAT");
	event->AddArgument((void*) slot, IPCTypes::uint);
	event->AddArgument((void*) message, IPCTypes::ch);
	
	rabbithole->SetEventForBroadcast(event);
	
	rabbithole->SignalEventSend();
}