#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "glm/glm.hpp"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

constexpr int FPS = 60;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
    bool isRunning = false;
    bool isDebugging = false;
    int millisecondsPreviousFrame = 0;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    Registry registry;
    AssetStore assetStore;
    EventBus eventBus;

public:
    Game();
    ~Game();
    void Initialize();
    void ProcessInput();
    void Setup();
    void LoadLevel(int level);
    void Update();
    void Render();
    void Run();
    void Destroy();

    int windowWidth;
    int windowHeight;
};

#endif