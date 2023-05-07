#include "logger.test.h"
#include "ecs.test.h"

int main() {
    testLogger();
    testAddEntityToSystem();
    testRemoveEntityFromSystem();
    return 0;
}