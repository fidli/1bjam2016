 #include "common.h"
 
 extern "C" {
     
     __declspec(naked) void _dtol3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _dtoui3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _dtoul3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _ftol3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _ftoui3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _ftoul3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _ltod3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _ultod3()
     {
         INV;
         __asm
         {
         }
     }
     
     __declspec(naked) void _alldiv(){
         INV;
     }
     
     __declspec(naked) void _allmul(){
         INV;
     }
     
     __declspec(naked) void _allrem(){
         INV;
     }
     
 }
 
 extern "C"{
#pragma function(memset)
     void * memset(void * dest, int c, size_t count)
     {
         char * bytes = (char *) dest;
         while (count--)
         {
             (*bytes) = (char) c;
             (*bytes++);
         }
         return dest;
     }
     
#pragma function(memcpy)
     void *memcpy(void *dest, const void *src, size_t count)
     {
         char *dest8 = (char *)dest;
         const char *src8 = (const char *)src;
         while (count--)
         {
             *dest8++ = *src8++;
         }
         return dest;
     }
 }
 
#include <Windows.h>
 extern "C"{
     int _fltused;
 };
#include "windows_types.h"
 
 
#define PERSISTENT_MEM MEGABYTE(250)
#define TEMP_MEM MEGABYTE(500)
#define STACK_MEM MEGABYTE(250)
 
 
#include "util_mem.h"
 
 struct Context{
     HINSTANCE hInstance;
     HWND window;
     bool appRunning;
 };
 
 
 Context context;
 
#include "util_string.cpp"
#include "windows_time.cpp"
#include "windows_render.cpp"
#include "windows_audio.cpp"
#include "gamecode.cpp"
 
 
 
 
 static inline DWORD jettisonAllPrivileges() {
     DWORD result = ERROR_SUCCESS;
     HANDLE processToken  = NULL;
     if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &processToken)){
         TOKEN_PRIVILEGES * priv = (TOKEN_PRIVILEGES *) mem.temp;
         DWORD privsize;
         if(GetTokenInformation(processToken, TokenPrivileges, priv , EFFECTIVE_TEMP_MEM_SIZE, &privsize) > 0){
             for(DWORD i = 0; i < priv->PrivilegeCount; ++i ){
                 priv->Privileges[i].Attributes = SE_PRIVILEGE_REMOVED;
             }
             if(AdjustTokenPrivileges(processToken, TRUE, priv, NULL, NULL, NULL) == 0){
                 result = GetLastError();
             }
         }else{
             result = GetLastError();
         }
     }else{
         result = GetLastError();
     }
     CloseHandle(processToken);
     return result;
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
             resizeCanvas(context.window, (WORD)lParam, (WORD) (lParam >> sizeof(WORD) * 8));
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
             
             EndPaint(context.window, &paint);
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
             context.appRunning = false;
             return 0;
         } break;
     }
     
     return DefWindowProc (window, message, wParam, lParam);
 }
 
 
 
 static inline int main(LPWSTR * argvW, int argc) {
     
     LPVOID memoryStart = VirtualAlloc(NULL, TEMP_MEM + PERSISTENT_MEM + STACK_MEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
     
     if (memoryStart) {
         initMemory(memoryStart);
         game = (World *) mem.persistent;
         HRESULT jettisonPrivilegesResult = jettisonAllPrivileges();
         ASSERT(jettisonPrivilegesResult == ERROR_SUCCESS);
         
         
         char ** argv = &PUSHA(char *, argc);
         
         for(int i = 0; i < argc; i++){
             int toAlloc = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
             ASSERT(toAlloc != 0);
             argv[i] = &PUSHA(char, toAlloc);
             int res = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], toAlloc, NULL, NULL);
             ASSERT(res != 0);
         }
         
         
         WNDCLASSEX style = {};
         style.cbSize = sizeof(WNDCLASSEX);
         style.style = CS_OWNDC;
         style.lpfnWndProc = WindowProc;
         style.hInstance = context.hInstance;
         style.lpszClassName = "GameMainClass";
         if(RegisterClassEx(&style) != 0){
             context.window = CreateWindowEx(NULL,
                                             "GameMainClass", "Paralyzed", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
                                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, context.hInstance, NULL);
             
             if(context.window){
                 if(initGraphics()){
                     if(initAudio()){
                         if(initTime()){
                             ShowWindow(context.window, SW_SHOWMAXIMIZED);
                             
                             initGameWorld();
                             
                             
                             float32 currentTime = getProcessCurrentTime();
                             float32 timeAccumulator = 0.0f;
                             
                             
                             
                             //-----------------------------------------------------------------------------------------------------------------------------------------------
                             while (context.appRunning) {
                                 float32 newTime = getProcessCurrentTime();
                                 float32 frameTime = newTime - currentTime;
                                 currentTime = newTime;
                                 timeAccumulator += frameTime;
                                 
                                 
                                 FLUSH;
                                 
                                 
                                 MSG msg;
                                 while(PeekMessage(&msg, context.window, 0, 0, PM_REMOVE))
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
                                 
                                 
                                 
                                 
                                 
                                 InvalidateRect(context.window, NULL, TRUE);                       
                             }
                             
                             
                             
                             
                             
                         }else{
                             //log
                             ASSERT(!"failed to init time");
                         }
                         closeAudio();
                     }else{
                         //log
                         ASSERT(!"failed to init audio");
                     }
                     closeGraphics();
                 }else{
                     //log
                     ASSERT(!"failed to init graphics");
                 }
                 
                 
             }else{
                 //log
                 ASSERT(!"failed to create window");
             }
             
             UnregisterClass("GameMainClass", context.hInstance);
         }
         
         
         
         if (!VirtualFree(memoryStart, 0, MEM_RELEASE)) {
             //more like log it
             ASSERT(!"Failed to free memory");
         }
         
         
     }else{
         //more like log it
         ASSERT(!"failed to init memory");
     }
     return 0;
     
     
 }
 
 
 
 void __stdcall WinMainCRTStartup(){
     //this might fail, consider, whether stop the app, or continue
     SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
     int argc = 0;
     LPWSTR * argv =  CommandLineToArgvW(GetCommandLineW(), &argc);
     
     context.hInstance = GetModuleHandle(NULL);
     context.appRunning = true;
     int result = main(argv,argc);
     LocalFree(argv);
     ExitProcess(result);
 }
 
