// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/World/Manager.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#ifdef MAXDOF_CLIENT_BUILD
#include "GameClient/Utils/Converters.h"
#endif

namespace MaxDoF::GameCommon::World
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

#ifdef MAXDOF_CLIENT_BUILD
    MaxDoF::GameClient::World::Graphics& Manager::getGraphics()
    {
        return mGraphics;
    }
#endif

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

        mEntities.reserve(maxBodies);
        mEntitiesIndex.reserve(maxBodies * 2);
        mEntitiesIndex.max_load_factor(static_cast<float>(maxBodies));

#ifdef MAXDOF_CLIENT_BUILD
        mGraphics.initialize();
#endif
    }

    void Manager::update(float timeSinceLast, const JPH::BodyID& shipBodyId)
    {
        if (!mPhysicsSystem)
            return;

JPH::RVec3 shipPosition = mPhysics.getBodyInterface()->GetPosition(shipBodyId);
        float physicsTimeStep = std::min(timeSinceLast, 0.1f);
        mPhysicsSystem->Update(physicsTimeStep, 1, mPhysicsAllocator, mJobSystem);
#ifdef MAXDOF_CLIENT_BUILD
        for (const auto& entity : mEntities)
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
#endif
    }

    Types::Entity& Manager::createEntity(
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* ogreNode,
#endif
        JPH::BodyID bodyId)
    {
        auto& entity = mEntities.emplace_back(bodyId
#ifdef MAXDOF_CLIENT_BUILD
            , ogreNode
#endif
        );
        mEntitiesIndex[bodyId.GetIndex()] = mEntities.size() - 1;

        return entity;
    }

    Types::Entity& Manager::createCube(const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createCube(MaxDoF::GameClient::Utils::PhysPoseToGraphPose(pose), MaxDoF::GameClient::Utils::JphVec3ToOgreVector3(halfSize)),
#endif
            mPhysics.createBox(pose, halfSize, mass));
    }

#ifdef MAXDOF_CLIENT_BUILD
    Ogre::SceneNode* Manager::createLight()
    {
        return mGraphics.createLight();
    }
#endif

    Types::Entity& Manager::createSphere(const Types::PhysPose& pose, float radius, float mass)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createSphere( MaxDoF::GameClient::Utils::PhysPoseToGraphPose( pose ), radius ),
#endif
            mPhysics.createSphere( pose, radius, mass ));
    }
}
