#include "ecs.test.h"
#include <iostream>
// uncoment to disable assert 
//#define NDEBUG 1
#include <cassert>

void printEntities(const std::vector<Entity>& entities) {
    for (auto &entity : entities) {
        std::cout << "| " << entity.GetId() << " |";        
    }
    std::cout << std::endl;
}

void testAddEntityToSystem() {
    System system;
    addEntitiesToSystem(system, 10);
    std::cout << "*** Entities *** " << std::endl;
    const auto& entities = system.GetSystemEntities();
    printEntities(entities);
    assert(( entities.size() == 10 )&&  "Should be 10 entities");
}

void testRemoveEntityFromSystem() {
    System system;
    addEntitiesToSystem(system, 10);
    std::cout << "*** Entities *** " << std::endl;
    const auto& entities = system.GetSystemEntities();
    printEntities(entities);

    Entity ent5(5);
    Entity ent8(8);
    Entity ent3(3);
    system.RemoveEntityFromSystem(ent5);
    system.RemoveEntityFromSystem(ent8);
    system.RemoveEntityFromSystem(ent3);

    printEntities(entities);

    assert((entities.size() == 7) && "Should be 7 entities");
}

void addEntitiesToSystem(System& system, int count) {
    for (int i = 0; i < count; i++) {
        Entity entity(i);
        system.AddEntityToSystem(entity);
    }
}