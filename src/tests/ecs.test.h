#ifndef ECS_TEST_H
#define ECS_TEST_H

#include "../ECS/ECS.h"

/*** TESTS ***/
void testAddEntityToSystem();
void testRemoveEntityFromSystem();

/*** HELPER FUNCTIONS ***/
void printEntities(const std::vector<Entity>& entities);
void addEntitiesToSystem(System& system, int count);

#endif