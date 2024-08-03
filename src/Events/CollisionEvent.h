#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class CollisionEvent : public Event
{
public:
    Entity a;
    Entity b;
    CollisionEvent(Entity a, Entity b) : a{a}, b{b}
    {
    }
};

#endif // COLLISIONEVENT_H