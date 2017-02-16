#ifndef WINDOWS_GUI_CPP
#define WINDOWS_GUI_CPP

#include "util_gui.h"

/*

static void resizeCanvas(HWND window, LONG width, LONG height){
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

static void updateCanvas(HDC dc, int x, int y, int width, int height){
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

*/
struct Window{
    HWND window;
    char className[128];
};


bool createWindow(const WindowParameters * parameters, Window * result){
    WNDCLASSEX style = {};
    style.cbSize = sizeof(WNDCLASSEX);
    style.style = CS_OWNDC;
    //style.lpfnWndProc = WindowProc;
    style.hInstance = context.hInstance;
    style.lpszClassName = parameters->name;
    strcpy(result->className, parameters->name);
    if(RegisterClassEx(&style) != 0){
        result->window = CreateWindowEx(NULL, parameters->name, parameters->name, WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, context.hInstance, NULL);
        return result->window != NULL;
    }else{
        return false;
    }
    return true;
}

bool destroyWindow(Window * target){
    bool32 res = DestroyWindow(target->window);
    return res && UnregisterClass(target->className, context.hInstance);
}

bool showWindow(Window * target){
    return ShowWindow(target->window, SW_SHOWMAXIMIZED);
}


#endif