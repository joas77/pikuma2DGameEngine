#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Utils/TilemapLoader.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyBoardmovementSystem.h"

Game::Game()
{
    Logger::Log("Game Constructor called");
}

Game::~Game()
{
    Logger::Log("Game Destructor called");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
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
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        Logger::Err("Error creating SDL window");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        Logger::Err("Error creating SDL renderer.");
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    isRunning = true;
}

void Game::Run()
{
    Setup();
    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }
            else if (sdlEvent.key.keysym.sym == SDLK_d)
            {
                isDebugging = !isDebugging;
            }
            eventBus.EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
            break;
        default:
            break;
        }
    }
}

void Game::LoadLevel(int level)
{
    // Add the systems that need to be processed in our game
    registry.AddSystem<MovementSystem>();
    registry.AddSystem<RenderSystem>();
    registry.AddSystem<AnimationSystem>();
    registry.AddSystem<CollisionSystem>();
    registry.AddSystem<RenderColliderSystem>();
    registry.AddSystem<DamageSystem>();
    registry.AddSystem<KeyBoardMovementSystem>();

    // Add assets to the asset store
    const std::string tankSpriteId = "tank-image";
    assetStore.AddTexture(renderer, tankSpriteId, "./assets/images/tank-panther-right.png");
    assetStore.AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    const auto tileMapPng = std::string{"./assets/tilemaps/jungle.png"};
    const auto tileMapSpriteId = std::string{"tile-map"};
    assetStore.AddTexture(renderer, tileMapSpriteId, tileMapPng);
    assetStore.AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
    assetStore.AddTexture(renderer, "radar-image", "./assets/images/radar.png");

    constexpr int tileSize = 32;
    constexpr double tileScale = 2.5;
    TileMapLoader tilemapLoader("./assets/tilemaps/jungle.map", "./assets/tilemaps/jungle.png", tileSize);

    auto map = tilemapLoader.getMap();
    for (const auto &tile : map)
    {
        auto tileBackground = registry.CreateEntity();
        tileBackground.AddComponent<TransformComponent>(
            glm::vec2(tileScale * tileSize * tile.relativePosition.x, tileScale * tileSize * tile.relativePosition.y),
            glm::vec2(tileScale, tileScale));
        tileBackground.AddComponent<SpriteComponent>(tileMapSpriteId, tileSize, tileSize, 0, tile.pixelSrcPosition.x, tile.pixelSrcPosition.y);
    }

    Entity chopper = registry.CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.5, 1.5), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", tileSize, tileSize, 3);
    chopper.AddComponent<AnimationComponent>(2, 15, true);

    Entity radar = registry.CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 100, 10.0), glm::vec2(1.5, 1.5), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = registry.CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.5, 1.5), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 0.0));
    tank.AddComponent<SpriteComponent>(tankSpriteId, tileSize, tileSize, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    Entity truck = registry.CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.5, 1.5), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", tileSize, tileSize, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
}

void Game::Setup()
{
    LoadLevel(1);
}

void Game::Update()
{
    // If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }

    // The difference in ticks since the last frame, converted to seconds.
    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

    // Store the current frame time
    millisecondsPreviousFrame = SDL_GetTicks();

    // Reset all event handlers for the current frame
    eventBus.Reset();

    // Perform the subscription of the events for all systems
    registry.GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry.GetSystem<KeyBoardMovementSystem>().SubscribeToEvents(eventBus);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry.Update();

    // Ask all the systems to update
    registry.GetSystem<MovementSystem>().Update(deltaTime);
    registry.GetSystem<AnimationSystem>().Update();
    registry.GetSystem<CollisionSystem>().Update(eventBus);
    registry.GetSystem<KeyBoardMovementSystem>().Update();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to render
    registry.GetSystem<RenderSystem>().Update(renderer, assetStore);
    if (isDebugging)
    {
        registry.GetSystem<RenderColliderSystem>().Update(renderer);
    }
    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}