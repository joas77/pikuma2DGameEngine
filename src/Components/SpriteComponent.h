#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>
#include "SDL2/SDL.h"

struct SpriteComponent {
    std::string assetId;
    int width;
    int height;
    int zIndex;
    SDL_Rect srcRect;

    SpriteComponent(
        const std::string& assetId = "",
        int width = 0, int height = 0,
        int zIndex = 0,
        int srcRectX = 0, int srcRectY = 0
        ) : 
        assetId{assetId},
        width{width}, height{height},
        zIndex{zIndex},
        srcRect{srcRectX, srcRectY, width, height}
    {}
    
};

#endif