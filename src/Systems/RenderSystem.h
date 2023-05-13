#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(SDL_Renderer* renderer, const AssetStore& assetStore) {

            auto lambda = [](const Entity& entA, const Entity& entB) {
                    const auto& spriteA = entA.GetComponent<SpriteComponent>();
                    const auto& spriteB = entB.GetComponent<SpriteComponent>();
                    return spriteA.zIndex < spriteB.zIndex;
            };
            sortEntities(lambda);

            // Loop all entities that system is interested in
            for(auto& entity : GetSystemEntities()) {
                const auto& transform = entity.GetComponent<TransformComponent>();
                const auto& sprite = entity.GetComponent<SpriteComponent>();

                // Set the source rectangle of our original sprite texture
                SDL_Rect srcRect = sprite.srcRect;

                // Set the destination rectangle with the x, y position to be rendered
                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer,
                    assetStore.GetTexture(sprite.assetId),
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    NULL,
                    SDL_FLIP_NONE
                );
            }
        }

};

#endif