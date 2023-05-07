#ifndef GAME_H
#define GAME_H

#include <iostream>
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "glm/glm.hpp"
#include "../AssetStore/AssetStore.h"

constexpr int FPS = 60;
constexpr int MILLISECS_PER_FRAME =  1000 / FPS;

class Game {
    private:
        bool isRunning;
        int millisecondsPreviousFrame = 0;
        SDL_Window* window;
        SDL_Renderer* renderer;

        std::unique_ptr<Registry> registry;
        AssetStore assetStore;

    public:
        Game();
        ~Game();
        void Initialize();
        void ProcessInput();
        void Setup();
        void Update();
        void Render();
        void Run();
        void Destroy();

        int windowWidth;
        int windowHeight;
};

#endif