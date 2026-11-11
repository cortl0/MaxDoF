// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/Manager.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#ifdef MAXDOF_CLIENT_BUILD
#include "Game/Utils/Converters.h"
#endif

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::World
{
    Manager::Manager()
#ifdef MAXDOF_CLIENT_BUILD
        : mGraphics("")
#endif
    {
    }

    Manager::~Manager()
    {
        if (mPhysicsSystem)
        {
            delete mPhysicsSystem;
            delete mJobSystem;
            delete mPhysicsAllocator;

            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }

    void Manager::initialize(unsigned int maxBodies)
    {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        mPhysicsAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10 Mb

        int numThreads = static_cast<int>(std::thread::hardware_concurrency()) - 1;
        numThreads = std::max(1, numThreads);

        mJobSystem = new JPH::JobSystemThreadPool(
            JPH::cMaxPhysicsJobs,
            JPH::cMaxPhysicsBarriers,
            numThreads
        );

        // ИЗМЕНЕНО: Выделяем объекты фильтров в куче (их адреса теперь не изменятся)
        mBroadPhaseLayerInterface = std::make_unique<MyBroadPhaseLayerInterface>();
        mObjectVsBroadPhaseLayerFilter = std::make_unique<MyObjectVsBroadPhaseLayerFilter>();
        mObjectLayerPairFilter = std::make_unique<MyObjectLayerPairFilter>();

        mPhysicsSystem = new JPH::PhysicsSystem();

        mPhysicsSystem->Init(
            maxBodies, 0, maxBodies, maxBodies,
            *mBroadPhaseLayerInterface,
            *mObjectVsBroadPhaseLayerFilter,
            *mObjectLayerPairFilter
        );

        mPhysics.setBodyInterface(&mPhysicsSystem->GetBodyInterface());
        mPhysicsSystem->SetGravity(JPH::Vec3::sZero());
        mEntities.Reserve(maxBodies);

#ifdef MAXDOF_CLIENT_BUILD
        mGraphics.initialize();
#endif
    }

    void Manager::update(float timeSinceLast, const JPH::BodyID& shipBodyId)
    {
        if (!mPhysicsSystem)
            return;

        float physicsTimeStep = std::min(timeSinceLast, 0.1f);
        mPhysicsSystem->Update(physicsTimeStep, 1, mPhysicsAllocator, mJobSystem);
        JPH::RVec3 shipPosition = mPhysics.getBodyInterface()->GetPosition(shipBodyId);
#ifdef MAXDOF_CLIENT_BUILD
        for (const auto& entity : mEntities.GetEntities())
        {
            if (!entity.graphNode)
                continue;

            JPH::RVec3 position;
            JPH::Quat rotation;
            mPhysics.getBodyInterface()->GetPositionAndRotation(entity.physId, position, rotation);
            JPH::Vec3 localPos = JPH::Vec3(position - shipPosition);
            entity.graphNode->setPosition(localPos.GetX(), localPos.GetY(), localPos.GetZ());
            entity.graphNode->setOrientation(Ogre::Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()));
        }
        mGraphics.setPositionOfGlobalStaticNode(Game::Utils::JphRVec3ToOgreVector3(-shipPosition));
#endif
    }

    Types::Entity& Manager::createEntity(
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* ogreNode,
#endif
        JPH::BodyID bodyId)
    {
         mEntities.AddEntity(bodyId
#ifdef MAXDOF_CLIENT_BUILD
            , ogreNode
#endif
        );

        return mEntities.GetEntities().back();
    }

    void Manager::destroyEntity(JPH::uint32 bodyIndex)
    {
        auto entity = mEntities.RemoveEntity(bodyIndex);

        if(!entity.has_value())
            return;

        mPhysics.destroyBody(entity.value().physId);

#ifdef MAXDOF_CLIENT_BUILD
        mGraphics.destroyNodeRecursively(entity.value().graphNode);
#endif
    }

    Types::Entity& Manager::createCube(JPH::uint64 userData, const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool onCCD)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createCube(MaxDoF::Game::Utils::PhysPoseToGraphPose(pose), MaxDoF::Game::Utils::JphVec3ToOgreVector3(halfSize)),
#endif
            mPhysics.createBox(userData, pose, halfSize, mass, dynamic, onCCD));
    }

    Types::Entity& Manager::createMesh(const Common::Types3d::MeshData& meshData)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createMesh(meshData),
#endif
            mPhysics.createMesh(meshData));
    }

    Types::Entity& Manager::createSphere(JPH::uint64 userData, const Types::PhysPose& pose, float radius, float mass, bool onCCD)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createSphere( MaxDoF::Game::Utils::PhysPoseToGraphPose( pose ), radius ),
#endif
            mPhysics.createSphere(userData, pose, radius, mass, onCCD));
    }
}
