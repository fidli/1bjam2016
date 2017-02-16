#ifndef UTIL_GUI_H
#define UTIL_GUI_H
//supporting unique windows only with its own contexts 


struct WindowParameters{
    char name[128];
    
};


struct Window;

bool createWindow(const WindowParameters * parameters, Window * result);

bool destroyWindow(Window * target);

bool showWindow(Window * target);


#endif