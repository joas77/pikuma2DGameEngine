#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(EventBus &eventBus)
    {
        auto &entities = GetSystemEntities();
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
            auto entityA = *it;
            for (auto loopIt = it; loopIt != entities.end(); ++loopIt)
            {
                if (it == loopIt)
                    continue;
                auto entityB = *loopIt;
                if (entityA != entityB &&
                    entityA.HasComponent<BoxColliderComponent>() &&
                    entityB.HasComponent<BoxColliderComponent>() &&
                    isCollision(entityA, entityB))
                {
                    eventBus.EmitEvent<CollisionEvent>(entityA, entityB);
                }
            }
        }
    }

private:
    // AABB (axis-aligned bounding boxes) collision detection
    bool isCollision(const Entity &entA, const Entity &entB)
    {
        const auto &tComponentA = entA.GetComponent<TransformComponent>();
        const auto &tComponentB = entB.GetComponent<TransformComponent>();
        const auto &colliderComponentA = entA.GetComponent<BoxColliderComponent>();
        const auto &colliderComponentB = entB.GetComponent<BoxColliderComponent>();

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