#include <windows.h>
#include <windowsx.h>
#include <Wingdi.h>
#include <Gdiplus.h> 
#include <DSound.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>


#define TOTAL_MEM GIGABYTE(1)
#define TEMP_MEM MEGABYTE(50)
#define STACK_MEM MEGABYTE(250)

#include "common.h"


inline void alertError(const char * text, HWND window = NULL){
	MessageBox(window, "You are fucked.", text, MB_ICONSTOP | MB_OK);
}

memory mem;



#include "game_math.cpp"
#include "gamecode.cpp"
#include "windows_game_render.cpp"




static bool appRunning = true;


inline Float64 getCurrentTime(){
	LARGE_INTEGER counter;
	LARGE_INTEGER frequency;
	if(QueryPerformanceCounter(&counter) == 0){
		alertError("Performance counter fucked up. (No microtime for you).");
		return 0.0f;
	};
	if(QueryPerformanceFrequency(&frequency) == 0){
		alertError("Performance frequency fucked up. (No microtime for you).");
		return 0.0f;
	};

  return (Float64) ((Float64)counter.QuadPart / (Float64)frequency.QuadPart);
}



void resizeCanvas(HWND window, LONG width, LONG height){
    if(renderer.DIBSection){
        DeleteObject(renderer.DIBSection);
    }else{
        renderer.backbufferDC = CreateCompatibleDC(NULL);
    }
    
        renderer.drawinfo = {{
        sizeof(BITMAPINFOHEADER),
        width,
        height,
        1,
        32,
        BI_RGB,
        0,
        0,
        0,
        0,
        0},
        0
       };
       renderer.DIBSection = CreateDIBSection(renderer.backbufferDC, &renderer.drawinfo, DIB_RGB_COLORS, (void**) &renderer.drawbuffer, NULL, NULL);
       renderer.height = height;
       renderer.width = width;
}

void updateCanvas(HDC dc, int x, int y, int width, int height){
    StretchDIBits(dc, x, y, width, height, 0, 0, width, height, renderer.drawbuffer, &renderer.drawinfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	    switch(message)
	    {
            case WM_SIZE:{
                resizeCanvas(window, (WORD)lParam, (WORD) (lParam >> sizeof(WORD) * 8));
            }
            break;
            case WM_PAINT:{
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(window, &paint);
                int x = paint.rcPaint.left;
                int y = paint.rcPaint.top;
                int width = paint.rcPaint.right - paint.rcPaint.left;
                int height = paint.rcPaint.bottom - paint.rcPaint.top;
                
                updateCanvas(dc, x, y, width, height);
                
                EndPaint(window, &paint);
            }
            break;
            case WM_KEYUP:{
                if(wParam == VK_SPACE){
                input.wasDown = input.isDown;
                input.isDown = false;
                }
            }
            break;
            case WM_KEYDOWN:{
                if(wParam == VK_SPACE){                
                input.wasDown = input.isDown;
                input.isDown = true;
                }
            }
            break;
			case WM_CLOSE:
	        case WM_DESTROY:
	            {
								appRunning = false;
								return 0;
	            } break;
	    }

	    return DefWindowProc (window, message, wParam, lParam);
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    LPVOID memoryStart = VirtualAlloc(NULL, TOTAL_MEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		HWND window = NULL;

    if (memoryStart) {
						mem.persistent = memoryStart;
                       mem.temp = (void *) ((Byte *) mem.persistent + TOTAL_MEM - TEMP_MEM);
                       mem.stack = (void *) ((Byte *) mem.persistent + TOTAL_MEM - TEMP_MEM - STACK_MEM);
        
        game = (World *) mem.persistent;
        ASSERT(sizeof(World) < TOTAL_MEM - TEMP_MEM - STACK_MEM);

						WNDCLASSEX style = {};
						style.cbSize = sizeof(WNDCLASSEX);
						style.style = CS_OWNDC;
						style.lpfnWndProc = WindowProc;
						style.hInstance = hInstance;
						style.lpszClassName = "GameMainClass";
						if(RegisterClassEx(&style) != 0){
							window = CreateWindowEx(NULL,
								 						"GameMainClass", "1BJAM: Paralyzed", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
	                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

						}
                      
                        

						if(window){
                            Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
    ULONG_PTR gdiplusToken; 
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 
                            

                            
																	ShowWindow(window, SW_SHOWMAXIMIZED);
                                                                    
                            //init screen? menus and shit?
                            
                            initGameWorld();
                            
                            
                            
																	Float64 currentTime = getCurrentTime();
																	Float64 timeAccumulator = 0.0f;
                            
                            
                       
    
                            int samplesPerSecond = 48000;
                            int bytesPerSample = sizeof(Int16)*2;
                            int bufferSize = samplesPerSecond*bytesPerSample*2;
 
                            LPDIRECTSOUND8 d8;
                            ASSERT(DirectSoundCreate8(0, &d8, 0) == DS_OK);
                            ASSERT(d8->SetCooperativeLevel(window, DSSCL_PRIORITY) == DS_OK);
                            
                         

                            DSBUFFERDESC pbuffdesc= {};
                            pbuffdesc.dwSize = sizeof(DSBUFFERDESC);
                            pbuffdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                                           

                            LPDIRECTSOUNDBUFFER primarybuffer;
                            
                            ASSERT(d8->CreateSoundBuffer(&pbuffdesc, &primarybuffer, 0) == DS_OK);                            
                            
                            WAVEFORMATEX wf = {};
                            wf.wFormatTag = WAVE_FORMAT_PCM;
                            wf.nChannels = 2;
                            wf.nSamplesPerSec = samplesPerSecond;
                            wf.wBitsPerSample = 16;
                            wf.nBlockAlign = (wf.nChannels*wf.wBitsPerSample) / 8;
                            wf.nAvgBytesPerSec = wf.nSamplesPerSec*wf.nBlockAlign;
                            wf.cbSize = 0;
                            
    
                            ASSERT(SUCCEEDED(primarybuffer->SetFormat(&wf)));
                            
                             DSBUFFERDESC sbuffdesc= {};
                            sbuffdesc.dwSize = sizeof(DSBUFFERDESC);
                             sbuffdesc.dwBufferBytes = bufferSize;
                             sbuffdesc.lpwfxFormat = &wf;
                                           

                            LPDIRECTSOUNDBUFFER secondarybuffer;
                            
                             ASSERT(SUCCEEDED(d8->CreateSoundBuffer(&sbuffdesc, &secondarybuffer, 0)));
                            
                             void * region1;
                             DWORD region1size;
                             void * region2;
                             DWORD region2size;
                             
                             
                             DWORD writeCursor;
                             DWORD playCursor;

                            Uint32 running = 0;
                             
                          
                           
                             secondarybuffer->Play(0,0,DSBPLAY_LOOPING);
     
//-----------------------------------------------------------------------------------------------------------------------------------------------
																	while (appRunning) {
																			Float64 newTime = getCurrentTime();
																			Float64 frameTime = newTime - currentTime;
																			currentTime = newTime;
																			timeAccumulator += frameTime;

                                                                        mem.tempOffset = 0;//reseting temp memory
                                                                        
                                                                        
																		MSG msg;
																			while(PeekMessage(&msg, window, 0, 0, PM_REMOVE))
																			{
																							TranslateMessage(&msg);
																							DispatchMessage(&msg);
																			}
                                                                 
                                          
                                                                            while(timeAccumulator >= TIME_STEP){
                                                                                update(TIME_STEP);
                                                                                timeAccumulator -= TIME_STEP;
                                                                            }
                                                                            
                                                                         
                                                                        
                                                                            render(getRenderQueue());
                                      ASSERT(SUCCEEDED(secondarybuffer->GetCurrentPosition(&playCursor, &writeCursor)));
                             
                             DWORD byteToLock = (running*bytesPerSample) % bufferSize;
                             DWORD bytesToWrite;                             

                                                         
                                                                            
                                                         //this is filling also not moving cursor, check that it has moved between frames, then fill                   
                             if(byteToLock >= playCursor){

                                 bytesToWrite = bufferSize - byteToLock; 
                                 bytesToWrite += playCursor;
                             } else{
                                 bytesToWrite = playCursor - byteToLock;
                             }
                             

                            
                          
                                                                            
                                                                            if(SUCCEEDED(secondarybuffer->Lock(byteToLock, bytesToWrite, &region1, &region1size, &region2, &region2size,0))){
                                
                            
                            
                             

                             int hz = 256;
                             int period =samplesPerSecond/hz;
                             
                             Int16 * lsample = (Int16 *) region1;
                          
                             DWORD sampleCount1 = region1size/bytesPerSample; 
                             DWORD sampleCount2 = region2size/bytesPerSample; 
                             

                             
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
                     
                             
                             ASSERT(SUCCEEDED(secondarybuffer->Unlock(region1, region1size, region2, region2size)));
                         }
            
 
                                InvalidateRect(window, NULL, TRUE);                       
                                                                        }



//------------------------------------------------------------------------------------------------------------------------------------------------
                            Gdiplus::GdiplusShutdown(gdiplusToken);                                             
                                         
							DestroyWindow(window);
                                                                      
                                                                        DeleteObject(renderer.DIBSection);
                                                                        DeleteDC(renderer.backbufferDC);
							UnregisterClass("EditorMainClass", hInstance);

		    				}



							if (!VirtualFree(memoryStart, 0, MEM_RELEASE)) {
								alertError("Failed to free memory", window);
							}

							return 0;
						}else{
							alertError("Failed to create window", window);
							return 0;
						}
						return 0;



}
