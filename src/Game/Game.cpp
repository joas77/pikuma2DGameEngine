#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Utils/TilemapLoader.h"

Game::Game() : 
    isRunning{false}
{
    registry = std::make_unique<Registry>();
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

void Game::LoadLevel(int level) {
    // Add the systems that need to be processed in our game
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // Add assets to the asset store
    const std::string tankSpriteId = "tank-image";
    assetStore.AddTexture(renderer, tankSpriteId, "./assets/images/tank-panther-right.png");
    assetStore.AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    const auto tileMapPng = std::string{"./assets/tilemaps/jungle.png"};
    const auto tileMapSpriteId = std::string{"tile-map"};
    assetStore.AddTexture(renderer, tileMapSpriteId, tileMapPng);

    constexpr int tileSize = 32;
    constexpr int scale = 2;
    TileMapLoader tilemapLoader("./assets/tilemaps/jungle.map", "./assets/tilemaps/jungle.png", tileSize);
    std::vector<Entity> backgroundEntities;
    auto map = tilemapLoader.getMap();
    for (const auto& tile : map) {
        auto tileBackground = registry->CreateEntity();
        tileBackground.AddComponent<TransformComponent>(
            glm::vec2(tileSize * tile.relativePosition.x, tileSize * tile.relativePosition.y),
            glm::vec2(1, 1)
        );
        tileBackground.AddComponent<SpriteComponent>(tileMapSpriteId, tileSize, tileSize, tile.pixelSrcPosition.x, tile.pixelSrcPosition.y);
        backgroundEntities.push_back(tileBackground);
    }

    // Create some entities
    Entity tank = registry->CreateEntity();

    // Add some components to that entity
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(scale, scale), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>(tankSpriteId, tileSize, tileSize);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(scale, scale), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>("truck-image", tileSize, tileSize);
}

void Game::Setup() {
    LoadLevel(1);
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