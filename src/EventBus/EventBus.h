#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <map>
#include <typeindex>
#include <memory>
#include <list>
#include <functional>
#include "../Logger/Logger.h"
#include "Event.h"

class IEventCallback
{
public:
    virtual ~IEventCallback() = default;
    void Execute(Event &e)
    {
        Call(e);
    }

private:
    virtual void Call(Event &e) = 0;
};

template <typename TOwner, typename TEvent>
using CallbackFunction = std::function<void(TOwner &, TEvent &)>;

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
    // Here we have the callback function pointer
    // that needs to be invoked
public:
    // typedef void (TOwner::*CallbackFunction)(TEvent &);
    EventCallback(TOwner &ownerInstance, std::function<void(TOwner &, TEvent &)> callbackFuncion) : ownerInstance{ownerInstance}, mCallbackFunction{callbackFuncion}
    {
    }

    virtual ~EventCallback() override = default;

private:
    TOwner &ownerInstance;
    CallbackFunction<TOwner, TEvent> mCallbackFunction;
    virtual void Call(Event &e) override
    {
        std::invoke(mCallbackFunction, ownerInstance, static_cast<TEvent &>(e));
    }
};

using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus
{
public:
    EventBus()
    {
        Logger::Log("Event bus constructor called!");
    }

    ~EventBus()
    {
        Logger::Log("EventBus destructor called!");
    }

    // Clears the subscribers list
    void Reset()
    {
        subscribers.clear();
    }

    /*
     * Subscribe to an event type <T>
     * A listener subscribes to an event
     * Example: eventBus.SubscribeToEvent<CollisionEvent>(this, &Game::onCollision)
     */
    template <typename TOwner, typename TEvent>
    void SubscribeToEvent(TOwner &ownerInstance, void (TOwner::*callbackFunction)(TEvent &))
    {
        if (subscribers.find(typeid(TEvent)) == subscribers.end())
        {
            subscribers[typeid(TEvent)] = HandlerList{};
        }

        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[typeid(TEvent)].push_back(std::move(subscriber));
    }

    /*
     * Emit an event of type <T>
     * As soon as something emits an event
     * we go ahed and execute all the listener callback functions
     * Example: eventBus.EmitEvent<CollisionEvent>(player, entity);
     */
    template <typename TEvent, typename... TArgs>
    void EmitEvent(TArgs &&...args)
    {
        auto &handlers = subscribers[typeid(TEvent)];
        for (const auto &h : handlers)
        {
            TEvent event{std::forward<TArgs>(args)...};
            h->Execute(event);
        }
    }

private:
    std::map<std::type_index, HandlerList> subscribers;
};

#endif // EVENTBUS_H