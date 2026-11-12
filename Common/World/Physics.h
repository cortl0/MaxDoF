// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Common/Types/Poses.h"
#include "Common/Types/Types3d.h"

namespace JPH
{
    class TempAllocatorImpl;
    class JobSystemThreadPool;
    class BodyInterface;
}

namespace MaxDoF::Common::World
{
    class MyBroadPhaseLayerInterface;
    class MyObjectVsBroadPhaseLayerFilter;
    class MyObjectLayerPairFilter;

    class Physics
    {
        JPH::TempAllocatorImpl* mPhysicsAllocator{ nullptr };
        JPH::JobSystemThreadPool* mJobSystem{ nullptr };
        JPH::PhysicsSystem* mPhysicsSystem{ nullptr };
        JPH::BodyInterface* mBodyInterface{ nullptr };
        std::unique_ptr<MyBroadPhaseLayerInterface> mBroadPhaseLayerInterface;
        std::unique_ptr<MyObjectVsBroadPhaseLayerFilter> mObjectVsBroadPhaseLayerFilter;
        std::unique_ptr<MyObjectLayerPairFilter> mObjectLayerPairFilter;

    public:
        Physics();
        ~Physics();

        JPH::BodyID createBox(Common::Types3d::EntityType userData, const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool onCCD);
        JPH::BodyID createMesh(const Types3d::MeshData&);
        JPH::BodyID createSphere(Common::Types3d::EntityType userData, const Types::PhysPose& pose, float radius, float mass, bool onCCD);
        void destroyBody(JPH::BodyID);
        JPH::BodyInterface* getBodyInterface() { return mBodyInterface; }
        JPH::PhysicsSystem* getPhysicsSystem(){ return mPhysicsSystem; }
        void initialize(unsigned int maxBodies = 1024);
        void setBodyInterface(JPH::BodyInterface* bodyInterface) { mBodyInterface = bodyInterface; }
        JPH::BodyID setBodySettings(Common::Types3d::EntityType userData, const JPH::ShapeSettings::ShapeResult& ShapeResult, const Types::PhysPose& pose, float mass, bool dynamic, bool onCCD);
        void update(float timeSinceLast);
    };
}
