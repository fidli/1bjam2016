#include <windows.h>
#include <windowsx.h>




#define TOTAL_MEM GIGABYTE(1)
#define TEMP_MEM MEGABYTE(250)
#define STACK_MEM MEGABYTE(250)

#include "common.h"


inline void alertError(const char * text, HWND window = NULL){
	MessageBox(window, "You are fucked.", text, MB_ICONSTOP | MB_OK);
}

memory mem;
HWND window;


#include "game_math.cpp"


#include "gamecode.cpp"
#include "windows_game_render.cpp"
#include "windows_game_audio.cpp"


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
                            
                            if(initGraphics()){
                                if(initAudio()){
                            
   
																	ShowWindow(window, SW_SHOWMAXIMIZED);
                                                                    
                            //init screen? menus and shit?
                            
                            initGameWorld();
                            
                            
                            
																	Float64 currentTime = getCurrentTime();
																	Float64 timeAccumulator = 0.0f;
                            
                            
          
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
                                                                            mix(getAudioQueue());
                                                                            
                                                          
                                                                            
                                                                            
                                                                                   
                                                                            InvalidateRect(window, NULL, TRUE);                       
                                                                        }



//------------------------------------------------------------------------------------------------------------------------------------------------
                                                                        closeAudio();
                                                                    }else{
                                                                    alertError("Failed to init sound", window);
                                                                    }
                                                                        closeGraphics();
                                                                    }else{
                                     								alertError("Failed to init graphics", window);
                                                                    }
                                         
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
