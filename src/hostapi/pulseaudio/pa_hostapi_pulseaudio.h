#ifndef _PA_HOSTAPI_PULSEAUDIO_H_
#define _PA_HOSTAPI_PULSEAUDIO_H_

#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"

#include "pa_unix_util.h"
#include "pa_ringbuffer.h"
#include "pa_debugprint.h"

/* Pulseaudio headers */
#include <stdio.h>
#include <string.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* prototypes for functions declared in this file */

#define PA_PULSEAUDIO_SET_LAST_HOST_ERROR(errorCode, errorText) \
    PaUtil_SetLastHostErrorInfo(paInDevelopment, errorCode, errorText)


#define PULSEAUDIO_TIME_EVENT_USEC 50000
#define PULSEAUDIO_BUFFER_SIZE (88100 * 4 * 2)

typedef struct
{
    PaUtilHostApiRepresentation inheritedHostApiRep;
    PaUtilStreamInterface callbackStreamInterface;
    PaUtilStreamInterface blockingStreamInterface;

    PaUtilAllocationGroup *allocations;

    PaHostApiIndex hostApiIndex;
    PaDeviceInfo deviceInfoArray[1024];
    char *pulseaudioDeviceNames[1024];

    /* Pulseaudio stuff goes here */
    pa_threaded_mainloop *mainloop;
    pa_context *context;
    int deviceCount;
    pa_context_state_t state;
    pa_time_event *timeEvent;
}
PaPulseaudioHostApiRepresentation;

/* PaPulseaudioStream - a stream data structure specifically for this implementation */

typedef struct PaPulseaudioStream
{
    PaUtilStreamRepresentation streamRepresentation;
    PaUtilCpuLoadMeasurer cpuLoadMeasurer;
    PaUtilBufferProcessor bufferProcessor;
    PaPulseaudioHostApiRepresentation *hostapi;

    PaUnixThread thread;
    unsigned long framesPerHostCallback;
    pa_threaded_mainloop *mainloop;
    pa_simple *simple;
    pa_context *context;
    pa_sample_spec outSampleSpec;
    pa_sample_spec inSampleSpec;
    pa_stream *outStream;
    pa_stream *inStream;
    size_t writableSize;
    pa_usec_t outStreamTime;
    pa_buffer_attr bufferAttr;
    int underflows;
    int latency;

    int callbackMode;              /* bool: are we running in callback mode? */
    int rtSched;
    long maxFramesPerBuffer;
    long maxFramesHostPerBuffer;
    int outputFrameSize;
    int inputFrameSize;

    PaDeviceIndex device;

    void *outBuffer;
    void *inBuffer;

    PaUtilRingBuffer        inputRing;
    PaUtilRingBuffer        outputRing;

    /* Used in communication between threads */
    volatile sig_atomic_t callback_finished; /* bool: are we in the "callback finished" state? */
    volatile sig_atomic_t callbackAbort;    /* Drop frames? */
    volatile sig_atomic_t isActive;         /* Is stream in active state? (Between StartStream and StopStream || !paContinue) */
    volatile sig_atomic_t isStopped;        /* Is stream in active state? (Between StartStream and StopStream || !paContinue) */

}
PaPulseaudioStream;

PaError PaPulseaudio_Initialize(PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index);

static void Terminate(struct PaUtilHostApiRepresentation *hostApi);


static PaError IsFormatSupported(struct PaUtilHostApiRepresentation *hostApi,
                                 const PaStreamParameters *inputParameters,
                                 const PaStreamParameters *outputParameters,
                                 double sampleRate);

static PaError OpenStream(struct PaUtilHostApiRepresentation *hostApi,
                          PaStream** s,
                          const PaStreamParameters *inputParameters,
                          const PaStreamParameters *outputParameters,
                          double sampleRate,
                          unsigned long framesPerBuffer,
                          PaStreamFlags streamFlags,
                          PaStreamCallback *streamCallback,
                          void *userData);


static PaError IsStreamStopped(PaStream *s);
static PaError IsStreamActive(PaStream *stream);

static PaTime GetStreamTime(PaStream *stream);
static double GetStreamCpuLoad(PaStream* stream);

PaPulseaudioHostApiRepresentation *PulseaudioNew(void);
void PulseaudioFree(PaPulseaudioHostApiRepresentation *ptr);

int PulseaudioCheckConnection(PaPulseaudioHostApiRepresentation *ptr);

static void PulseaudioCheckContextStateCb(pa_context * c, void *userdata);
void PulseaudioSinkListCb(pa_context *c, const pa_sink_info *l, int eol, void *userdata);
void PulseaudioSourceListCb(pa_context *c, const pa_source_info *l, int eol, void *userdata);

void PulseaudioStreamStateCb(pa_stream *s, void *userdata);
void PulseaudioStreamStartedCb(pa_stream *s, void *userdata);
void PulseaudioStreamUnderflowCb(pa_stream *s, void *userdata);

PaError PulseaudioConvertPortaudioFormatToPulseaudio(PaSampleFormat portaudiosf,
        pa_sample_spec *pulseaudiosf);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

