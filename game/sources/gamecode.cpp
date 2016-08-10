#include "game_structures.h"

void loadImage(const  WCHAR * file, Image * target);

void initGameWorld(){
       //open map file, parse map, loading range of possible nodes according to gameplay tree
    
    game->currentNode = 0;
   
    game->nodes[0];
                                               
    loadImage(L"D:\\Git\\1bjam2016\\game\\bin\\x64\\images\\01_01.JPG", &game->nodes[0].originalImage);
    
    game->nodes[0].cachedScaledImage.pixeldata = 0;
                  
 
}


RenderItem * getRenderQueue(){
    RenderItem * first = &PUSH(RenderItem);
    
    first->original = &game->nodes[game->currentNode].originalImage;
    first->cached = &game->nodes[game->currentNode].cachedScaledImage;
    first->x = first->y = 0.0f;
    first->w = first->h = 1.0f;
    first->next = 0;
    
    return first;
}