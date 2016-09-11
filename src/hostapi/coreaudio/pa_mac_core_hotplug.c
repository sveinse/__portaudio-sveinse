/*
	Hotplug notification system for Core Audio on Mac.
		Notifies on device list changes and default I/O changes.
*/

#include <pthread.h>

#include "pa_hotplug.h"

#include "pa_mac_core_internal.h"

typedef struct PaMacCoreHotplugState
{
	// We can store hotplug notifier state in this structure if needed.
	int dummy;
	
	pthread_mutex_t mutex;
}
	PaMacCoreHotplugState;

static PaMacCoreHotplugState paMacCoreHotplugState;


static OSStatus PaMacDevicesChanged
	(AudioHardwarePropertyID inPropertyID, void *userData)
{
	//PaMacCoreHotplugState *hotplugState = (PaMacCoreHotplugState*) userData;
	
	// Dispatch the notification.
	PaUtil_DevicesChanged(0, NULL);
	
	return noErr;
}


void PaUtil_InitializeHotPlug()
{
	OSStatus err = noErr;
	
	// Set up a non-recursive mutex for notification locking
	pthread_mutex_init(&paMacCoreHotplugState.mutex, NULL);
	
	// Add property listeners
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDevices,             &PaMacDevicesChanged, &paMacCoreHotplugState);
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultInputDevice,  &PaMacDevicesChanged, &paMacCoreHotplugState);
	err |= AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultOutputDevice, &PaMacDevicesChanged, &paMacCoreHotplugState);
	
	if (err)
	{
		VVDBUG(("PaUtil_InitializeHotPlug(): failed to register listener"));
	}
}
void PaUtil_TerminateHotPlug()
{
	// Remove property listeners
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDevices,             &PaMacDevicesChanged);
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDefaultInputDevice,  &PaMacDevicesChanged);
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDefaultOutputDevice, &PaMacDevicesChanged);
	
	pthread_mutex_destroy(&paMacCoreHotplugState.mutex);
}

void PaUtil_LockHotPlug()
{
	pthread_mutex_lock(&paMacCoreHotplugState.mutex);
}
void PaUtil_UnlockHotPlug()
{
	pthread_mutex_unlock(&paMacCoreHotplugState.mutex);
}
