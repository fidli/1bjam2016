#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

struct Image{
    Uint32 width;
    Uint32 height;
    Uint32 * pixeldata;
    
};

struct RenderItem{
    Image * original;
    Image * cached;
    Float32 x, y;
    Float32 w, h;
    RenderItem * next;
};


struct StoryNode{
    Image originalImage;
    Image cachedScaledImage;
    Uint16 follow[4];
};

struct World{
    Uint16 currentNode;
    StoryNode nodes[2000];
};



struct PlayerInput{

};

World * game;

#endif