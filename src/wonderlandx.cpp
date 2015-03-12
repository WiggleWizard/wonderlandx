#include <stdio.h>
#include <unistd.h>
#include <pinc.h>

#include "globals.h"
#include "ipc_broker.h"

PCL int OnInit()
{
	Plugin_Printf("Initialising WonderlandX\n");
	
	IPCBroker* ipcChan = new IPCBroker();
	
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
	strncpy(info->shortDescription,"Wonderland for CoD4X17a",sizeof(info->shortDescription));
}

