#include "game_structures.h"


void parseDialog(const char * text, Dialog * target){
    int currentSentence = 0;
    int currentWord = 0;
    
    int textLength = strlen(text);
    
    int tempOffset = 0;
    
    for(int textOffset = 0; tempOffset < textLength; tempOffset++){
        
        if(!strcmp(text + tempOffset, "...")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, "...");
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, "?!")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, "?!");
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, "?")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, "?!");
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, "!")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, "!");
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, ".")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, ".");
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, ",")){
            target->sentence[currentSentence].wordsCount = currentWord + 1;
            
            strcpy(target->sentence[currentSentence].end, ",");
            
            textOffset = tempOffset+2;
            currentWord = 0;
            currentSentence++;
        }else if(!strcmp(text + tempOffset, " ")){
            memcpy(target->sentence[currentSentence].words[currentWord], text + textOffset, tempOffset - textOffset);
            
            textOffset = tempOffset+1;
            currentWord++;
            
        }
        
        
    }
    
    target->sentencesCount = currentSentence + 1;
}


void setCurrentNode(int node){
    game->currentNode = node;
    game->requiresBlink = game->nodes[node].followCount > 1;
    game->isChoosing = false;
    game->nodeChanged = true;
    
}

void initGameWorld(){
    //open map file, parse map, loading range of possible nodes according to gameplay tree
    
    loadAudio(L"D:\\Git\\1bjam2016\\game\\build\\images\\first_wakeup.m4a", &game->nodes[0].talk);
    FLUSH;
    
    loadImage(L"D:\\Git\\1bjam2016\\game\\build\\images\\01_01.JPG", &game->nodes[0].originalImage);
    FLUSH;
    game->nodes[0].cachedScaledImage.pixeldata = 0;
    game->nodes[0].followCount = 1;
    parseDialog("Are you fucking kidding me? This is some horseshit! Linebreak you shit. Fucker.", &game->nodes[0].dialog);
    FLUSH;
    game->nodes[0].follow[0] = 1;
    
    
    
    loadAudio(L"D:\\Git\\1bjam2016\\game\\build\\images\\remember_we_were_drinking.m4a", &game->nodes[1].talk);
    FLUSH;
    loadImage(L"D:\\Git\\1bjam2016\\game\\build\\images\\01_02.JPG",
              &game->nodes[1].originalImage);
    FLUSH;
    game->nodes[1].cachedScaledImage.pixeldata = 0;
    game->nodes[1].followCount = 1;
    
    game->nodes[1].follow[0] = 2;
    
    game->nodes[2] = game->nodes[0];
    game->nodes[2].followCount = 1;
    
    game->nodes[2].follow[0] = 3;
    
    loadImage(L"D:\\Git\\1bjam2016\\game\\build\\images\\01_03.JPG", &game->nodes[3].originalImage);
    FLUSH;
    game->nodes[3].cachedScaledImage.pixeldata = 0;
    game->nodes[3].followCount = 0;
    
    setCurrentNode(0);
    
}

void update(float32 dt){
    if(game->nodeChanged){
        game->nodeChanged = false;
    }
    if(game->timers.blink <= 0.0f){
        game->isBlinking = false;
    }
    if(game->nodes[game->currentNode].talk.finished){
        
        uint8 choice = 0;
        
        if(game->requiresBlink){
            game->isChoosing = true;
            //gief choice
        }else{
            if(game->nodes[game->currentNode].followCount > 0)
            {
                ASSERT(choice < game->nodes[game->currentNode].followCount);
                
                setCurrentNode(game->nodes[game->currentNode].follow[choice]);
            }
        }
    }
    
    
    
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
    AudioItem * queue = NULL;
    
    if(game->nodeChanged){
        queue = &PUSH(AudioItem);
        queue->next = NULL;
        queue->type = AudioType_PlayTalk;
        queue->target = &game->nodes[game->currentNode].talk;
        
    }
    if(game->isChoosing){
        queue = &PUSH(AudioItem);
        queue->next = NULL;
        queue->type = AudioType_StopTalk;
        
    }
    return queue;
}

RenderItem * getRenderQueue(){
    RenderItem * target = 0;
    
    RenderItem * first = &PUSH(RenderItem);
    
    target = first;
    
    first->type = RenderType_Image;
    first->image.original = &game->nodes[game->currentNode].originalImage;
    first->image.cached = &game->nodes[game->currentNode].cachedScaledImage;
    first->image.x = first->image.y = 0.0f;
    first->image.w = first->image.h = 1.0f;
    
    RenderItem * subtitle = &PUSH(RenderItem);
    subtitle->type = RenderType_Text;
    
    // subtitle->text.text = game->nodes[game->currentNode].talktext;
    
    
    target->next = subtitle;
    target = subtitle;
    
    target->next = 0;
    
    if(game->isBlinking){
        RenderItem * blink = target->next = &PUSH(RenderItem);
        blink->type = RenderType_BlinkEffect;
        blink->blink.progress = sin(((BLINK_TIME - game->timers.blink)/BLINK_TIME)*PI);
        target = blink;
        target->next = 0;
    }
    
    
    
    return first;
}