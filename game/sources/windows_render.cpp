#ifndef WINDOWS_RENDER_H
#define WINDOWS_RENDER_H


#include <Wingdi.h>
#include <Gdiplus.h> 


#include "util_image.cpp"



struct DrawContext{
    uint32 * drawbuffer;
    BITMAPINFO drawinfo;
    HDC  backbufferDC;
    HBITMAP DIBSection;
    uint32 width;
    uint32 height;
};


DrawContext renderer;
Gdiplus::GdiplusStartupInput * gdiplusStartupInput; 
ULONG_PTR  gdiplusToken; 


static union ColorConvertor{
    uint32 complex;
    struct{
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    } c;
};

static inline void loadImage(const WCHAR * file, Image * target){
    Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromFile(file);
    
    target->width = ((Gdiplus::Image *) image)->GetWidth();
    target->height = ((Gdiplus::Image *) image)->GetHeight();
    
    target->pixeldata = &PPUSHA(uint32, target->width*target->height) 
    
    
        for(int w = 0; w < target->width; w++){
        for(int h = 0; h < target->height; h++){
            Gdiplus::Color c;
            image->GetPixel(w, h, &c);
            ColorConvertor col;
            col.c.r = c.GetRed();
            col.c.g = c.GetGreen();
            col.c.b = c.GetBlue();
            col.c.a = c.GetAlpha();
            int i = h*target->width  + w;
            target->pixeldata[i] = col.complex;
        }
    }
    
    delete image;
}

bool initGraphics(){
    //fecking constructors
    gdiplusStartupInput = &PPUSH(Gdiplus::GdiplusStartupInput);
    Gdiplus::GdiplusStartupInput tmp;
    *gdiplusStartupInput = tmp;
    return Gdiplus::GdiplusStartup(&gdiplusToken, gdiplusStartupInput, NULL) == 0; //Status.OK
}

void closeGraphics(){
    Gdiplus::GdiplusShutdown(gdiplusToken); 
}

enum RenderType{
    RenderType_Image,
    RenderType_BlinkEffect,
    RenderType_Text
};


struct RenderItem{
    RenderType type;
    RenderItem * next;
    union{
        struct {
            Image * original;
            Image * cached;
            float32 x, y;
            float32 w, h;
        } image;
        struct {
            float32 progress;
        } blink;
    };
};

void render(RenderItem * queue){
    //do not bother clearing previous buffer, because we are always drawing full pic,
    //in case of resizing, or aspect ratio draws, we might be fucked, consider clearing first
    RenderItem * current = queue;
    while(current){
        //maybe switch differned render commands
        switch(current->type){
            case RenderType_Image:{
                
                uint32 targetW = current->image.w * renderer.width;
                uint32 targetH = current->image.h * renderer.height;
                uint32 offsetX = current->image.x * renderer.width;
                uint32 offsetY = current->image.y * renderer.height;
                
                //scale and save
                if(current->image.cached->pixeldata == 0 || targetW != current->image.cached->width || targetH != current->image.cached->height){
                    //prepare memory
                    
                    //note many scaling could potentionally create holes, also, in the future, better memory management, maybe linked list mem?
                    current->image.cached->pixeldata = &PPUSHS(uint32, targetW*targetH*4);
                    scaleImage(current->image.original, current->image.cached, targetW, targetH);
                    
                }
                
                //render cached
                for(int w = offsetX; w < renderer.width && w < offsetX + targetW; w++){
                    for(int h = offsetY; h < renderer.height && h < offsetY + targetH; h++){
                        int buffi = (renderer.height - h - 1)*renderer.width  + w;
                        int srci = (w-offsetX) + (h-offsetY)*targetW;
                        //also, fuck blending for now
                        renderer.drawbuffer[buffi] = current->image.cached->pixeldata[srci];
                    }
                }
                
            } break; 
            case RenderType_BlinkEffect:{
                uint32 black = 0x00000000;                                
                ASSERT(current->blink.progress >= 0 && current->blink.progress <= 1);
                uint32 Ysize = (renderer.height/2) * current->blink.progress;
                for(int w = 0; w < renderer.width; w++){
                    for(int h = 0; h <= Ysize; h++){
                        int buffi = (renderer.height - h - 1)*renderer.width  + w;         
                        renderer.drawbuffer[buffi] = black;
                    }
                    for(int h = renderer.height-1; h >= renderer.height-Ysize; h--){
                        int buffi = (renderer.height - h - 1)*renderer.width  + w;         
                        renderer.drawbuffer[buffi] = black;                        
                    }
                }                
            } break;
            default: ASSERT(!"Invalid");
        }
        current = current->next;
    }    
}

#endif
