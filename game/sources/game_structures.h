#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

#define TIME_STEP 0.02f

struct Font{
    Image source;
    uint32 gridWidth;
    uint32 gridHeight;
};

struct Dialog{
    struct {
        char words[10][30];
        char end[6];
        uint8 wordsCount;
    } sentence[10];
    uint8 sentencesCount;
};


struct StoryNode{
    Image originalImage;
    Image cachedScaledImage;
    Audio talk;
    uint8 followCount;
    Dialog dialog;
    uint16 follow[4];
};

#define BLINK_TIME 0.5f

struct World{
    uint16 currentNode;
    bool requiresBlink;
    bool nodeChanged;
    
    StoryNode nodes[2000];
    bool isBlinking;
    bool isChoosing;
    struct {
        float32 blink;
        
    }timers;
};



struct PlayerInput{
    bool isDown;
    bool wasDown;
};

PlayerInput input;

World * game;

#endif