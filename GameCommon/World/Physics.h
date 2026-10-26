// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameCommon/Types/Poses.h"

namespace MaxDoF::GameCommon::World
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
        JPH::BodyID setBodySettings(const JPH::ShapeSettings::ShapeResult& ShapeResult, const Types::PhysPose& pose, float mass, bool dynamic);
        JPH::BodyID createBox(const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic);
        JPH::BodyID createSphere(const Types::PhysPose& pose, float radius, float mass);
    };
}
