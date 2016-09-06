/*
	Stub implementation of hotplug notification system for Mac
 */

#include "pa_hotplug.h"

#include "pa_mac_core_internal.h"

typedef struct PaMacCoreHotplugState
{
	int dummy;
}
	PaMacCoreHotplugState;

static PaMacCoreHotplugState paMacCoreHotplugState;


static OSStatus PaMacCoreHardwarePropertyListener
	(AudioHardwarePropertyID inPropertyID, void *userData)
{
	//PaMacCoreHotplugState *hotplugState = (PaMacCoreHotplugState*) userData;
	
	//Notify!
	PaUtil_DevicesChanged(0, NULL);
	
	return noErr;
}


void PaUtil_InitializeHotPlug()
{
	OSStatus err = noErr;
	
	// Add property listener
	err = AudioHardwareAddPropertyListener(
		kAudioHardwarePropertyDevices,
		&PaMacCoreHardwarePropertyListener,
		&paMacCoreHotplugState);
	
	if (err)
	{
		VVDBUG(("PaUtil_InitializeHotPlug(): failed to register listener"));
	}
}
void PaUtil_TerminateHotPlug()
{
	// Remove property listener
	AudioHardwareRemovePropertyListener(
		kAudioHardwarePropertyDevices,
		&PaMacCoreHardwarePropertyListener);
}

void PaUtil_LockHotPlug() {}
void PaUtil_UnlockHotPlug() {}
