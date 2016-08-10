#ifndef WINDOWS_GAME_RENDER_H
#define WINDOWS_GAME_RENDER_H

static union ColorConvertor{
                                Uint32 complex;
                                struct{
                                    Uint8 b;
                                    Uint8 g;
                                    Uint8 r;
                                    Uint8 a;
                                } c;
                            };

                            static struct NearestNeighbourColor{
                                Uint32 color;
                                Uint8 times;
                            };
                            
                            
                            struct DrawContext{
                            Uint32 * drawbuffer;
BITMAPINFO drawinfo;
HDC  backbufferDC;
HBITMAP DIBSection;
Uint32 width;
Uint32 height;
                            };
                            


DrawContext renderer;

#endif
