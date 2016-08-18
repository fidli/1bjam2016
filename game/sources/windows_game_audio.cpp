#include "windows_game_audio.h"

//intended only for loading audio by mff bollocks
static struct buffersample{
    buffersample * next;
    Byte * data;
    DWORD size;
};

void loadAudio(const WCHAR * path, Audio * target){
    IMFSourceReader * reader;
    HRESULT r =  MFCreateSourceReaderFromURL(path, NULL, &reader);
    ASSERT(r == S_OK);
    
    //deselect all streams
    ASSERT(SUCCEEDED(reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE)));
    //select first audio stream
    ASSERT(SUCCEEDED(reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE)));
    
    //set the target type, #noidea
    IMFMediaType * targetType;
    ASSERT(SUCCEEDED(MFCreateMediaType(&targetType)));
    ASSERT(SUCCEEDED(targetType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)));
    ASSERT(SUCCEEDED(targetType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)));
    
    //convert
    reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, targetType);
    
    //obtain exact variables
    reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &targetType);
    
    WAVEFORMATEX * sourceFormat;
    UINT32 formatSize = 0;
    ASSERT(SUCCEEDED(MFCreateWaveFormatExFromMFMediaType(targetType, &sourceFormat, &formatSize)));
    
    ASSERT(sourceFormat->nChannels == 2);
    ASSERT(sourceFormat->nSamplesPerSec == mixer.samplesPerSecond);
    ASSERT(sourceFormat->wBitsPerSample == 16);
    
    //copy data into own memory
    //(is roughly 10 MB/sec)
    DWORD outflags;
    IMFSample * sample;
    IMFMediaBuffer *buffer;
    buffersample * queue = &PUSH(buffersample);
    buffersample * current = queue;
    DWORD totalSize = 0;
     while(true){
         //read samples till you die,
         ASSERT(SUCCEEDED(reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &outflags, NULL, &sample)));
         
         if((outflags & MF_SOURCE_READERF_ENDOFSTREAM) || sample == NULL){
             current->next = NULL;
             break;
         }
         
         ASSERT(SUCCEEDED(sample->ConvertToContiguousBuffer(&buffer)));
         
        
         Byte * data;
         DWORD currentSize = 0;
         ASSERT(SUCCEEDED(buffer->Lock(&data, NULL, &currentSize)));
         
         totalSize += currentSize;
         current->size = currentSize;
         
         current->data = &PUSHS(Byte, currentSize);
         
         for(int i = 0; i < currentSize; i++){
             current->data[i] = data[i];
         }
         
         ASSERT(SUCCEEDED(buffer->Unlock()));
         
             current->next = &PUSH(buffersample);
             current = current->next;
         buffer->Release();
         sample->Release();
    }
    
    //copy linked list audio chunks into one piece of memory
    target->samplesAmount = totalSize / 4; //2x 16 bits
    target->samplesdata = &PPUSHS(Int32, target->samplesAmount);
    
    Byte * samplesbuffer = (Byte *) target->samplesdata;
    
    current = queue;
    
    while(current != NULL){
        for(int i = 0; i < current->size; i++){
            *samplesbuffer = current->data[i];
            samplesbuffer++;
        }
        current = current->next;
    }
    
    
    CoTaskMemFree(sourceFormat);
    targetType->Release();
    reader->Release();
       
}

bool initAudio(){

    mixer. samplesPerSecond = 44100;
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