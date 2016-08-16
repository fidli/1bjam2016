#include "game_structures.h"

void loadImage(const  WCHAR * file, Image * target);
void loadAudio(const  WCHAR * file);

void setCurrentNode(int node){
    game->currentNode = node;
    game->requiresBlink = game->nodes[node].followCount > 1;
    game->timers.node = game->nodes[node].timer;
}

void initGameWorld(){
       //open map file, parse map, loading range of possible nodes according to gameplay tree
    
    loadAudio(L"D:\\Git\\1bjam2016\\game\\bin\\x64\\images\\first_wakeup.m4a");
    
                            
    loadImage(L"D:\\Git\\1bjam2016\\game\\bin\\x64\\images\\01_01.JPG", &game->nodes[0].originalImage);
    game->nodes[0].cachedScaledImage.pixeldata = 0;
    game->nodes[0].followCount = 1;
    game->nodes[0].timer = 1.0f;
    game->nodes[0].follow[0] = 1;
    
    
    
    loadImage(L"D:\\Git\\1bjam2016\\game\\bin\\x64\\images\\01_02.JPG", &game->nodes[1].originalImage);
    game->nodes[1].cachedScaledImage.pixeldata = 0;
    game->nodes[1].followCount = 1;
    game->nodes[1].timer = 1.0f;
    game->nodes[1].follow[0] = 2;
    
    game->nodes[2] = game->nodes[0];
    game->nodes[2].followCount = 1;
    game->nodes[2].timer = 1.0f;
    game->nodes[2].follow[0] = 3;

    loadImage(L"D:\\Git\\1bjam2016\\game\\bin\\x64\\images\\01_03.JPG", &game->nodes[3].originalImage);
    game->nodes[3].timer = 1.0f;
    game->nodes[3].cachedScaledImage.pixeldata = 0;
    game->nodes[3].followCount = 0;

        setCurrentNode(0);
 
}

void update(Float32 dt){
    if(game->timers.blink <= 0.0f){
        game->isBlinking = false;
    }
    if(game->timers.node <= 0.0f){
    
    Uint8 choice = 0;
    
    if(game->requiresBlink){
    //gief choice
    }else{
        if(game->nodes[game->currentNode].followCount > 0)
        {
        ASSERT(choice < game->nodes[game->currentNode].followCount);
            
        setCurrentNode(game->nodes[game->currentNode].follow[choice]);
        }
      }
    }
    
    game->timers.node -= dt;
    
    if(input.isDown && !input.wasDown && !game->isBlinking){
        game->isBlinking = true;
        game->timers.blink = BLINK_TIME;
    }else if(game->isBlinking){
       game->timers.blink -= dt;
      if(game->timers.blink <= 0.0f){
        game->timers.blink = 0.0f;
    }  
    }
    
    
    
  }



  AudioItem * getAudioQueue(){
      return NULL;
  }

RenderItem * getRenderQueue(){
    RenderItem * first = &PUSH(RenderItem);
    
    first->type = RenderType_Image;
    first->image.original = &game->nodes[game->currentNode].originalImage;
    first->image.cached = &game->nodes[game->currentNode].cachedScaledImage;
    first->image.x = first->image.y = 0.0f;
    first->image.w = first->image.h = 1.0f;
    first->next = 0;
    
    if(game->isBlinking){
    RenderItem * blink = first->next = &PUSH(RenderItem);
    blink->type = RenderType_BlinkEffect;
   blink->blink.progress = sin(((BLINK_TIME - game->timers.blink)/BLINK_TIME)*PI);
    blink->next = 0;
    }
    
    
    
    return first;
}