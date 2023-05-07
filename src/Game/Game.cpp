#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() : 
    isRunning{false}
{
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game Constructor called");
}

Game::~Game() {
    Logger::Log("Game Destructor called");
}

void Game::Initialize() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );

    if(!window) {
        Logger::Err("Error creating SDL window");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        Logger::Err("Error creating SDL renderer.");
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    isRunning = true;
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
            break;
        default:
            break;
        }
    }
}

void Game::Setup() {
    // Add the systems that need to be processed in our game
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // Add assets to the asset store
    const std::string tankSpriteId = "tank-image";
    assetStore->AddTexture(renderer, tankSpriteId, "./assets/images/tank-panther-right.png");
                                                   
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");

    // Create some entities
    Entity tank = registry->CreateEntity();

    // Add some components to that entity
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>(tankSpriteId, 32, 32);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32);

}

void Game::Update() {
    // If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
    if(timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    // The difference in ticks since the last frame, converted to seconds.
    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;
    
    // Store the current frame time
    millisecondsPreviousFrame = SDL_GetTicks();
    
    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Ask all the systems to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to render
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}