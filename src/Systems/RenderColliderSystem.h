#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class RenderColliderSystem: public System {
    public:
        RenderColliderSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer* renderer) {
            for(auto& entity : GetSystemEntities()) {
                const auto& tComp = entity.GetComponent<TransformComponent>();
                const auto& colliderComp = entity.GetComponent<BoxColliderComponent>();
                        
                SDL_Rect bbox = {
                    static_cast<int>(tComp.position.x + colliderComp.offset.x),
                    static_cast<int>(tComp.position.y + colliderComp.offset.y),
                    static_cast<int>(colliderComp.width * tComp.scale.x),
                    static_cast<int>(colliderComp.height * tComp.scale.y)
                };

                SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
                SDL_RenderDrawRect(renderer, &bbox);    
                          
            }
        }
};

#endif