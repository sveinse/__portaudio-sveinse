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

static PaMacCoreHotplugState paMacHotplugState;


static OSStatus PaMacDevicesChanged
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
	
	// Add property listeners
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDevices,             &PaMacDevicesChanged, &paMacHotplugState);
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultInputDevice,  &PaMacDevicesChanged, &paMacHotplugState);
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultOutputDevice, &PaMacDevicesChanged, &paMacHotplugState);
	
	if (err)
	{
		VVDBUG(("PaUtil_InitializeHotPlug(): failed to register listener"));
	}
}
void PaUtil_TerminateHotPlug()
{
	// Remove property listener
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDevices,             &PaMacDevicesChanged);
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDefaultInputDevice,  &PaMacDevicesChanged);
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDefaultOutputDevice, &PaMacDevicesChanged);
}

void PaUtil_LockHotPlug() {}
void PaUtil_UnlockHotPlug() {}
