#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void Update(double deltaTime) {
            // Loop all entities that the system is interested in
            for(auto& entity : GetSystemEntities()) {
                // Update entity position based on its velocity
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

            }
        }
};

#endif