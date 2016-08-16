#ifndef WINDOWS_GAME_AUDIO_H
#define WINDOWS_GAME_AUDIO_H

#include <DSound.h>
#include <Mfidl.h>
#include <Mfapi.h>
#include <Objbase.h>
#include <Mfreadwrite.h>

struct audio{
                            int samplesPerSecond;
                            int bytesPerSample;
                            int bufferSize;
    
    LPDIRECTSOUNDBUFFER talkbuffer;
};

audio mixer;

#endif