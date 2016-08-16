#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

#define TIME_STEP 0.02f

struct Image{
    Uint32 width;
    Uint32 height;
    Uint32 * pixeldata;
    
};


enum RenderType{
RenderType_Image,
    RenderType_BlinkEffect
};

struct RenderItem{
    RenderType type;
    RenderItem * next;
    union{
        struct {
    Image * original;
    Image * cached;
    Float32 x, y;
    Float32 w, h;
        } image;
        struct {
            Float32 progress;
        } blink;
    };
};


enum AudioType{
    AudioType_PlayTalk,
    AudioType_StopTalk,
};

struct AudioItem{
    AudioType type;
    AudioItem * next;
};

struct StoryNode{
    Image originalImage;
    Image cachedScaledImage;
    Uint8 followCount;
    Float32 timer;
    Uint16 follow[4];
};

#define BLINK_TIME 0.5f

struct World{
    Uint16 currentNode;
    bool requiresBlink;
    
    StoryNode nodes[2000];
    bool isBlinking;
    struct {
        Float32 blink;
        Float32 node;
    }timers;
};



struct PlayerInput{
    bool isDown;
    bool wasDown;
};

PlayerInput input;

World * game;

#endif