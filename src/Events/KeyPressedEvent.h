#ifndef KEYPRESSED_EVENT_H
#define KEYPRESSED_EVENT_H

#include <SDL2/SDL_keycode.h>
#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class KeyPressedEvent : public Event
{
public:
    KeyPressedEvent(SDL_Keycode keycode) : keyPressed{keycode}
    {
    }

    SDL_Keycode keyPressed;
};

#endif // KEYPRESSED_EVENT_H