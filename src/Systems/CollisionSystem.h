#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class CollisionSystem: public System {
    public:
        CollisionSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update() {
            auto& entities = GetSystemEntities();
            for(auto it = entities.begin(); it != entities.end() - 1; ++it ) {
                const auto& entityA = *it;
                for(auto loopIt = it + 1; loopIt != entities.end(); ++loopIt) {
                    const auto& entityB = *loopIt;
                    if( entityA != entityB &&
                        entityA.HasComponent<BoxColliderComponent>() &&
                        entityB.HasComponent<BoxColliderComponent>() &&
                        isCollision(entityA, entityB))
                            Logger::Log("JULIO_DEBUG: collision detected! between entity " + 
                        std::to_string(it->GetId()) + " and " + std::to_string(loopIt->GetId())
                    );
                }
            }
        }

    private:
        // AABB (axis-aligned bounding boxes) collision detection
        bool isCollision(const Entity& entA, const Entity& entB) {
            const auto& tComponentA = entA.GetComponent<TransformComponent>();
            const auto& tComponentB = entB.GetComponent<TransformComponent>();
            const auto& colliderComponentA = entA.GetComponent<BoxColliderComponent>();
            const auto& colliderComponentB = entB.GetComponent<BoxColliderComponent>();

            const auto entAXmin = tComponentA.position.x + colliderComponentA.offset.x; 
            const auto entAXmax = entAXmin + colliderComponentA.width * tComponentA.scale.x;

            const auto entAYmin = tComponentA.position.y + colliderComponentA.offset.y;
            const auto entAYmax = entAYmin + colliderComponentA.height * tComponentA.scale.y;

            const auto entBXmin = tComponentB.position.x + colliderComponentB.offset.x; 
            const auto entBXmax = entBXmin + colliderComponentA.width * tComponentB.scale.x;

            const auto entBYmin = tComponentB.position.y + colliderComponentB.offset.y;
            const auto entBYmax = entBXmin + colliderComponentB.height * tComponentB.scale.y;

            return entAXmin <= entBXmax &&
                    entAXmax >= entBXmin &&
                    entAYmin <= entBYmax &&
                    entAYmax >= entBYmin;
        }

};

#endif