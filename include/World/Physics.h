// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_World_Physics_H_
#define MaxDoF_World_Physics_H_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>

// Обязательные интерфейсы фильтрации для Jolt 5.x+
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include "Utils/Types.h"
#include "World/Entity.h"

namespace MaxDoF::World
{
    class Physics
    {
        JPH::BodyInterface* mBodyInterface{ nullptr };

    public:
        Physics();
        ~Physics();

        void setBodyInterface(JPH::BodyInterface* bodyInterface) { mBodyInterface = bodyInterface; }
        JPH::BodyInterface* getBodyInterface() { return mBodyInterface; }
        void initialize(unsigned int maxBodies = 1024);
        JPH::BodyID setBodySettings(const JPH::ShapeSettings::ShapeResult& ShapeResult, const Types::PhysPose& pose, float mass);
        JPH::BodyID createBox(const Types::PhysPose& pose, const JPH::RVec3& halfSize, float mass);
        JPH::BodyID createSphere(const Types::PhysPose& pose, float radius, float mass);
    };
}  // namespace MaxDoF::World

#endif
