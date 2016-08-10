#include "windows_game_render.h"

void loadImage(const WCHAR * file, Image * target){
    Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromFile(file);
    
    target->width = ((Gdiplus::Image *) image)->GetWidth();
    target->height = ((Gdiplus::Image *) image)->GetHeight();
    
    target->pixeldata = &PPUSHS(Uint32, target->width*target->height*4) 
    

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



void scaleImage(const Image * source, Image * target, Uint32 targetWidth, Uint32 targetHeight){
    Float32 scaleX = (Float32)source->width/(Float32)targetWidth;
    Float32 scaleY = (Float32)source->height/(Float32)targetHeight;

    NearestNeighbourColor * nn = &PUSH(NearestNeighbourColor);
    
    target->width = targetWidth;
    target->height = targetHeight;

    ASSERT(source->width > targetWidth && source->height > targetHeight); //support scaling up later
    
    //each target pixel
for(int tw = 0; tw < target->width; tw++){
    for(int th = 0; th < target->height; th++){
        int i = target->width*th  + tw;//final index
        
        //clear NN
        for(int clr = 0; clr < scaleX*scaleY; clr++){
            nn[clr] = {};
        }
        Uint8 nncount = 0;
        
        //for all neighbours to the original pixel
        for(int nw = 0; nw < scaleX; nw++){
            int srcW = tw*scaleX + nw;
            for(int nh = 0; nh < scaleY; nh++){
                int srcH = th*scaleY + nh;
                
                Uint32 srcColor = source->pixeldata[srcH*source->width + srcW];                
                //look for same color;
                bool found = false;
                for(int s = 0; s < nncount; s++){
                    if(nn[s].color == srcColor){
                        nn[s].times++;
                        found = true;
                        break;
                    }
                }
                if(!found){
                    nn[nncount++].color = srcColor;
                }
                //do not overstep image boundaries, last neighbours could be a little bit off
                if(srcH > source->width){
                    break;
                }
            }
            //do not overstep image boundaries, last neighbours could be a little bit off
            if(srcW > source->height){
                break;
            }
        }
        
        Uint32 resultColor = 0xFFFFFFFF;
        Sint8  highest = -1;
        for(int s = 0; s < nncount; s++){
            if(nn[s].times > highest){
                resultColor = nn[s].color;
            }
        }
        target->pixeldata[i] = resultColor;
        
    }
}
                                                          
}

void render(RenderItem * queue){
    //do not bother clearing previous buffer, because we are always drawing full pic,
    //in case of resizing, or aspect ratio draws, we might be fucked, consider clearing first
    RenderItem * current = queue;
    while(current){
        //maybe switch differned render commands
        
        Uint32 targetW = current->w * renderer.width;
        Uint32 targetH = current->h * renderer.height;
        Uint32 offsetX = current->x * renderer.width;
        Uint32 offsetY = current->y * renderer.height;
        
        //scale and save
        if(current->cached->pixeldata == 0 || targetW != current->cached->width || targetH != current->cached->height){
            //prepare memory
            
            //note many scaling could potentionally create holes, also, in the future, better memory management, maybe linked list mem?
            current->cached->pixeldata = &PPUSHS(Uint32, targetW*targetH*4);
            scaleImage(current->original, current->cached, targetW, targetH);
    
        }
        
        //render cached
        for(int w = offsetX; w < renderer.width && w < offsetX + targetW; w++){
            for(int h = offsetY; h < renderer.height && h < offsetY + targetH; h++){
               int buffi = (renderer.height - h - 1)*renderer.width  + w;
                int srci = (w-offsetX) + (h-offsetY)*targetW;
                //also, fuck blending for now
                renderer.drawbuffer[buffi] = current->cached->pixeldata[srci];
            }
        }
        
        current = current->next;
    }    
}
