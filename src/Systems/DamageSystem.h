#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System
{
public:
    DamageSystem()
    {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(EventBus &eventBus)
    {
        eventBus.SubscribeToEvent<DamageSystem, CollisionEvent>(*this, &DamageSystem::onCollision);
    }

    void onCollision(CollisionEvent &event)
    {
        Logger::Log("The Damage system received an event collisions between entities " +
                    std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));
        event.a.Kill();
        event.b.Kill();
    }

    void Update()
    {
        // TODO ..
    }
};

#endif // DAMAGESYSTEM_H