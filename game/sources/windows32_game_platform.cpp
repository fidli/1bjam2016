extern "C" void * memset(void * dest, int c, size_t count)
{
    char * bytes = (char *) dest;
    while (count--)
    {
        (*bytes) = (char) c;
        (*bytes++);
    }
    return dest;
}

#include <Windows.h>
extern "C"{
    int _fltused;
};
#include "windows_types.h"


#define PERSISTENT_MEM GIGABYTE(1)
#define TEMP_MEM MEGABYTE(250)
#define STACK_MEM MEGABYTE(250)

#include "common.h"
#include "util_mem.h"

struct Context{
    HINSTANCE hInstance;
};


Context context;

#include "util_string.cpp"
#include "windows_time.cpp"
#include "windows_gui.cpp"
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


static inline int main(LPWSTR * argvW, int argc) {
    
    LPVOID memoryStart = VirtualAlloc(NULL, TEMP_MEM + PERSISTENT_MEM + STACK_MEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (memoryStart) {
        initMemory(memoryStart);
        HRESULT jettisonPrivilegesResult = jettisonAllPrivileges();
        ASSERT(jettisonPrivilegesResult == ERROR_SUCCESS);
        
        
        char ** argv = &PUSHA(char *, argc);
        
        for(int i = 0; i < argc; i++){
            int toAlloc = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
            ASSERT(toAlloc != 0);
            argv[i] = &PUSHA(char, toAlloc);
            ASSERT(WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], toAlloc, NULL, NULL) != 0);
        }
        
        
        run();
        
        
        
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
    int result = main(argv,argc);
    LocalFree(argv);
    ExitProcess(result);
}

