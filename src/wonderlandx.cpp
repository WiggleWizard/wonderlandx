#include <stdio.h>
#include <unistd.h>
#include <pinc.h>

#include "globals.h"
#include "ipc_rabbithole.h"
#include "ipc_event.h"
#include "logger.h"
#include "limbo_man.h"

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
	
	LimboMan::Instance()->SetLimboLimit(Plugin_GetSlotCount());
	
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

PCL void OnPlayerJoinReq(int clientnum, netadr_t* netaddress, char* pbguid, char* userinfo, int authstatus, char* deniedmsg, int deniedmsgbufmaxlen, qboolean* wait)
{
	bool isWaiting = LimboMan::Instance()->IsWaiting(clientnum);
	bool isDenied  = LimboMan::Instance()->IsDenied(clientnum);
	
	// Allow the main thread to continue with dealing with the connection request
	if(!isWaiting)
		*wait = qfalse;
	else
		*wait = qtrue;
	
	if(isDenied)
	{
		size_t deniedLen = strnlen(deniedmsg, MAX_STRING_CHARS);
		
		strncpy(deniedmsg, LimboMan::Instance()->GetDenyReason(clientnum), deniedLen);
		deniedmsgbufmaxlen = deniedLen;
	}
	
	// If the slot has not been accepted yet then send the event
	if(isWaiting)
	{
		const char* ipAddr = Plugin_NET_AdrToStringShort(netaddress);

		IPCEvent* event = new IPCEvent("JOINREQ");
		event->AddArgument((void*) clientnum, IPCTypes::uint);
		event->AddArgument((void*) ipAddr, IPCTypes::ch);
		event->AddArgument((void*) Plugin_GetPlayerGUID(clientnum), IPCTypes::ch);
		event->AddArgument((void*) userinfo, IPCTypes::ch);

		rabbithole->SetEventForBroadcast(event);

		rabbithole->SignalEventSend();
	}
	// If the slot was cleared to continue with dealing with the connection
	// then just reset this limbo.
	else
		LimboMan::Instance()->Reset(clientnum);
}

PCL void OnPlayerConnect(int clientnum, netadr_t* netaddress, char* pbguid, char* userinfo, int authstatus)
{
	const char* ipAddr = Plugin_NET_AdrToStringShort(netaddress);
	
	IPCEvent* event = new IPCEvent("CONNECT");
	event->AddArgument((void*) clientnum, IPCTypes::uint);
	event->AddArgument((void*) ipAddr, IPCTypes::ch);
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

PCL void OnPlayerDC(client_t* client, const char* reason)
{
	// Majik numberrr
	int clientnum = (int) client / 0x090b4f8c - 1;
	
	IPCEvent* event = new IPCEvent("DC");
	event->AddArgument((void*) clientnum, IPCTypes::uint);
	
	rabbithole->SetEventForBroadcast(event);
	rabbithole->SignalEventSend();
}