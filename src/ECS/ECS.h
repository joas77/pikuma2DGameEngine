#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include <memory>
#include <functional>
#include <algorithm>
#include "../Logger/Logger.h"

constexpr unsigned int MAX_COMPONENTS = 32;

//////////////////////////////////////////
// Signature
/////////////////////////////////////////////
// We use a bitset (1s and 0s ) to keep track of which components and entity has,
// and also helps keep track of which entities a system is interested in.
/////////////////////////////////////////////
using Signature = std::bitset<MAX_COMPONENTS>;

struct IComponent {
    protected:
        static std::size_t nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component : public IComponent {
    public:
        // Returns the unique id of Component<T>
        static std::size_t GetId() {
            static auto id = nextId++;
            return id;
        }
};

class Entity {
    private:
        std::size_t id;
    public:
        Entity(std::size_t id) : id(id) {};
        void Kill();
        std::size_t GetId() const;

        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const { return id == other.id; };
        bool operator !=(const Entity& other) const { return id != other.id; };
        bool operator >(const Entity& other) const { return id > other.id; };
        bool operator <(const Entity& other) const { return id < other.id; };

        template <typename TComponent, typename ...TArgs>
        void AddComponent(TArgs&& ...args);

        template <typename TComponent>
        void RemoveComponent();

        template <typename TComponent>
        bool HasComponent() const;

        template <typename TComponent>
        TComponent& GetComponent() const;


        // Hold a pointer to the entity's owner registry
        class Registry* registry;
};

/*******************************************
 System
******************************************
 The system process entities that contains a specific signature
*******************************************/
class System {
    private:
        Signature componentSignature;
        using EntitiesContainer = std::vector<Entity>;
        EntitiesContainer entities;

    protected:
        void sortEntities(std::function<bool(const Entity& , const Entity& )>&& lambda);

    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity>& GetSystemEntities();
        const Signature& GetComponentSignature() const;

        // Define the component type T that entities must have to be
        // considered by the system
        template <typename TComponent>
        void RequireComponent();
};

template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

///////////////////////////////////////////////////
// Pool
////////////////////////////////////////////////////
// A pool is just a vector (contiguos data) of objects of type T
////////////////////////////////////////////////////
class IPool {
    public:
        virtual ~IPool() {}
};

template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;

    public:
        Pool(int size = 100) {
            data.resize(size);
        }

        virtual ~Pool() = default;

        bool isEmpty() const {
            return data.empty();
        }

        std::size_t GetSize() const {
            return data.size();
        }

        void Resize(int n) {
            data.resize(n);
        }

        void Clear() {
            data.clear();
        }

        void Add(T object) {
            data.push_back(object);
        }

        void Set(std::size_t index, T object) {
            data[index] = object;
        }

        T& Get(std::size_t index) {
            return static_cast<T&>(data[index]);
        }

        T& operator [](std::size_t index) {
            return data[index];
        }
};

///////////////////////////////////////////////////
// Registry
////////////////////////////////////////////////////
// The registry manaes the creation adn destruction of entities,
// add systems and components
////////////////////////////////////////////////////
class Registry {
    private:
        std::size_t numEntities = 0;

        // Vector of component pools, each pool contains all the
        // data for a certain component type
        // Pool index = entity id
        std::vector<std::shared_ptr<IPool>> componentPools;

        // Vector of component signatures per entity,
        // saying which component is turned "on" for each entity
        // [ Vector index = entity id ]
        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        // Set of entities that are flagged to be added or removed the
        // next registry Update()
        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;

        // List of free entity ids that were previously removed
        std::deque<int> freeIds;

    public:
        Registry() {
            Logger::Log("Registry constructor called");
        }

        ~Registry() {
            Logger::Log("Registry destructor called.");
        }

        // The Registru Update finally process the entities that
        // are waiting to be added/killed
        void Update();

        // Entity management
        Entity CreateEntity();
        void KillEntity(Entity entity);
        
        // Component management
        template <typename TComponent, typename ...Targs>
        void AddComponent(Entity entity, Targs&& ...args);

        template <typename TComponent>
        void RemoveComponent(Entity entity);

        template <typename TComponent>
        bool HasComponent(Entity entity);

        template <typename TComponent>
        TComponent& GetComponent(Entity entity) const;

        void AddEntityToSystem(Entity entity);
        
        // System Management
        template <typename TSystem, typename ...Targs>
        void AddSystem(Targs& ...args);

        template <typename TSystem>
        void RemoveSystem();

        template <typename TSystem>
        bool HasSystem() const;

        template <typename TSystem>
        TSystem& GetSystem() const;

        // Add and remove entities from their systems
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);
};

template <typename TSystem, typename ...Targs>
void Registry::AddSystem(Targs& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<Targs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template<typename TComponent, typename ...Targs>
void Registry::AddComponent(Entity entity, Targs&& ...args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if(componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if(!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if(entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<Targs>(args)...);

    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return (entityId < entityComponentSignatures.size()) &&
        entityComponentSignatures[entityId].test(componentId);
    
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const{
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

#endif