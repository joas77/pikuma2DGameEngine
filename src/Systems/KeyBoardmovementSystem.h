#ifndef KEYBOARD_MOVEMENT_SYTEM_H
#define KEYBOARD_MOVEMENT_SYTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyBoardMovementSystem : public System
{
public:
    void SubscribeToEvents(EventBus &eventBus)
    {
        eventBus.SubscribeToEvent<KeyBoardMovementSystem, KeyPressedEvent>(*this, &KeyBoardMovementSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent &event)
    {
        std::string keyCode = std::to_string(event.keyPressed);
        std::string keySymbol = std::string(1, event.keyPressed);
        Logger::Log("Key pressed event emitted: [" + keyCode + "]" + keySymbol);
    }

    void Update()
    {
    }
};

#endif // KEYBOARD_MOVEMENT_SYTEM_H