#include "windows_game_audio.h"


void loadAudio(const WCHAR * path){
    IMFSourceReader * reader;
    HRESULT r =  MFCreateSourceReaderFromURL(path, NULL, &reader);
    ASSERT(r == S_OK);
    
    IMFMediaType * mediaType;
    
    ASSERT(SUCCEEDED(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &mediaType)));
    
    mediaType->Release();
    
    reader->Release();
       
}

bool initAudio(){

    mixer. samplesPerSecond = 48000;
    mixer. bytesPerSample = sizeof(Int16)*2;
    mixer. bufferSize = mixer.samplesPerSecond*mixer.bytesPerSample*2;
    
 
    LPDIRECTSOUND8 d8;
    if(SUCCEEDED(DirectSoundCreate8(0, &d8, 0))){
        if(SUCCEEDED(d8->SetCooperativeLevel(window, DSSCL_PRIORITY))){
                            
                          WAVEFORMATEX wf = {};
                            wf.wFormatTag = WAVE_FORMAT_PCM;
                            wf.nChannels = 2;
                            wf.nSamplesPerSec = mixer.samplesPerSecond;
                            wf.wBitsPerSample = 16;
                            wf.nBlockAlign = (wf.nChannels*wf.wBitsPerSample) / 8;
                            wf.nAvgBytesPerSec = wf.nSamplesPerSec*wf.nBlockAlign;
                            wf.cbSize = 0;

                            DSBUFFERDESC pbuffdesc= {};
                            pbuffdesc.dwSize = sizeof(DSBUFFERDESC);
                            pbuffdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                                           
                            LPDIRECTSOUNDBUFFER primarybuffer;
                            
                            if(SUCCEEDED(d8->CreateSoundBuffer(&pbuffdesc, &primarybuffer, 0))){
                               if(!SUCCEEDED(primarybuffer->SetFormat(&wf))){
                                   return false;
                               }
                              
                             
                           }                      else{
                               return false;
                           }      
                            
                           
                            
    
                            
                            
                             DSBUFFERDESC sbuffdesc= {};
                            sbuffdesc.dwSize = sizeof(DSBUFFERDESC);
                             sbuffdesc.dwBufferBytes = mixer.bufferSize;
                             sbuffdesc.lpwfxFormat = &wf;
                                           
                            
                            
                             if(!SUCCEEDED(d8->CreateSoundBuffer(&sbuffdesc, &mixer.talkbuffer, 0))){
                                 return false;
                             }
                             
                         }
                     }
                     
                     
                     //now for reading and loading files
                     
                      if(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) == S_OK){
                     if(SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_LITE))){
                          return true;
                        }
                     } 
                     
                     
                     
                         return false;
}

void closeAudio(){
//io files
    MFShutdown();
    CoUninitialize();
}


void mix(AudioItem * queue){
    AudioItem * current = queue;
    while(current != NULL){
        switch(current->type){
            case AudioType_PlayTalk:
            mixer.talkbuffer->Play(0,0,DSBPLAY_LOOPING);
            break;
            case AudioType_StopTalk:
            mixer.talkbuffer->Stop();
            break;
            default:
            ASSERT(!"FUCK");
            break;
        }
        current = current->next;
    }
}
/*
       void * region1;
                             DWORD region1size;
                             void * region2;
                             DWORD region2size;
                             
                             
                             DWORD writeCursor;
                             DWORD playCursor;

                            Uint32 running = 0;

                                      ASSERT(SUCCEEDED(mixer.talkbuffer->GetCurrentPosition(&playCursor, &writeCursor)));
                             
                             DWORD byteToLock = (running*mixer.bytesPerSample) % mixer.bufferSize;
                             DWORD bytesToWrite;                             

                                                         
                                                                            
                                                         //this is filling also not moving cursor, check that it has moved between frames, then fill                   
                             if(byteToLock >= playCursor){

                                 bytesToWrite = mixer.bufferSize - byteToLock; 
                                 bytesToWrite += playCursor;
                             } else{
                                 bytesToWrite = playCursor - byteToLock;
                             }
                             

                            
                          
                                                                            
                                                                            if(SUCCEEDED(mixer.talkbuffer->Lock(byteToLock, bytesToWrite, &region1, &region1size, &region2, &region2size,0))){
                                
                            
                            
                             

                             int hz = 256;
                             int period = mixer.samplesPerSecond/hz;
                             
                             Int16 * lsample = (Int16 *) region1;
                          
                             DWORD sampleCount1 = region1size/mixer.bytesPerSample; 
                             DWORD sampleCount2 = region2size/mixer.bytesPerSample; 
                             

                             
                             for(DWORD index = 0; index < sampleCount1; index++){
                                 
                                 Int16 val = ((running / (period/2))%2) ? 1500 : -1500;
                                 *lsample = val;
                                     lsample++;
                                 *lsample = val;
                                     lsample++;
                                 running++;

                             }
                             Int16 * llsample = (Int16 *) region2;
                             for(DWORD index = 0; index < sampleCount2; index++){
                                  
                                 Int16 val = ((running / (period/2))%2) ? 1500: -1500;
                                 *llsample = val;
                                     llsample++;
                                 *llsample = val;

                                 llsample++;
                                                                  running++; 
                             }
                     
                             
                             ASSERT(SUCCEEDED(mixer.talkbuffer->Unlock(region1, region1size, region2, region2size)));
                         }
            
 */