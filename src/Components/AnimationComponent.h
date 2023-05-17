#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SDL2/SDL.h>

struct AnimationComponent {
    int numFrames;
    int currentFrame;
    int frameSpeedRate;
    bool isLoop;
    int startTime;

    AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool isLoop = true):
        numFrames{numFrames}, currentFrame{1},
        frameSpeedRate{frameSpeedRate}, isLoop{isLoop}
    {
        startTime = SDL_GetTicks();
    }
};

#endif