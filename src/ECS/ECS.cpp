#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

std::size_t IComponent::nextId = 0;

std::size_t Entity::GetId() const {
    return id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&entity](Entity other) {
                return entity == other;
            }),
        entities.end()
    );
}

std::vector<Entity>& System::GetSystemEntities() {
    return entities;
}

void System::sortEntities(std::function<bool(const Entity&, const Entity&)>&& lambda) {
    std::sort(entities.begin(), entities.end(), lambda);
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

Entity Registry::CreateEntity() {
    std::size_t entityId;
    
    if(freeIds.empty()) {
        // if there are no free ids waiting to be reused
        entityId = numEntities++;
        if(entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // Reuse and id from the list of previously remove entities
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entityId));
    
    return entity;
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    const auto entityComponentSignature = entityComponentSignatures[entityId];

    // loop all the systems
    for(auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if(isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for(auto& system: systems) {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update() {
    // Add the entities that are waiting to be
    // created to the active systems
    for(auto entity: entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    // Process the entities that are waiting to be
    // killed from the active systems
    for(auto entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        
        entityComponentSignatures[entity.GetId()].reset();

        // Make the entity id available to be reused
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeKilled.clear();
}