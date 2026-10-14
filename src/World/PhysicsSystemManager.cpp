// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "World/PhysicsSystemManager.h"

#include <algorithm>
#include <thread>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreSceneNode.h>
#endif

#include "Utils/Converters.h"

namespace MaxDoF::World
{
    PhysicsSystemManager::PhysicsSystemManager()
    {
    }

    PhysicsSystemManager::~PhysicsSystemManager()
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

    void PhysicsSystemManager::initialize(unsigned int maxBodies)
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

        mBodyInterface = &mPhysicsSystem->GetBodyInterface();
        mPhysicsSystem->SetGravity(JPH::Vec3::sZero());

        mEntities.reserve(maxBodies);
        mEntitiesIndex.reserve(maxBodies * 2);
        mEntitiesIndex.max_load_factor(static_cast<float>(maxBodies));
    }

    void PhysicsSystemManager::update(float timeSinceLast)
    {
        if (!mPhysicsSystem) return;

        float physicsTimeStep = std::min(timeSinceLast, 0.1f);
        mPhysicsSystem->Update(physicsTimeStep, 1, mPhysicsAllocator, mJobSystem);

        for (const auto& entity : mEntities)
        {
#ifdef MAXDOF_CLIENT_BUILD
            if (!entity.graphNode) continue;
#endif

            JPH::RVec3 position;
            JPH::Quat rotation;
            mBodyInterface->GetPositionAndRotation(entity.physId, position, rotation);
#ifdef MAXDOF_CLIENT_BUILD
            entity.graphNode->setPosition(position.GetX(), position.GetY(), position.GetZ());
            entity.graphNode->setOrientation(Ogre::Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()));
#endif
        }
    }

    void PhysicsSystemManager::syncPhysicsToGraphics(JPH::BodyID bodyId, Ogre::SceneNode* ogreNode)
    {
        if (!mBodyInterface || !ogreNode) return;

        JPH::RVec3 position;
        JPH::Quat rotation;

        mBodyInterface->GetPositionAndRotation(bodyId, position, rotation);

        ogreNode->setPosition(position.GetX(), position.GetY(), position.GetZ());
        ogreNode->setOrientation(Ogre::Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()));
    }

    JPH::BodyID PhysicsSystemManager::setBodySettings(const JPH::ShapeSettings::ShapeResult& shapeResult, const Types::Pose& pose, float mass)
    {
        JPH::BodyCreationSettings bodySettings(
            shapeResult.Get(),
            pose.position,
            pose.orientation,
            JPH::EMotionType::Dynamic, JPH::ObjectLayer(0)
        );

        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = mass;
        bodySettings.mLinearDamping = 0.0f;
        bodySettings.mAngularDamping = 0.0f;

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID PhysicsSystemManager::createPhysBox(const Types::Pose& pose, const JPH::RVec3& halfSize, float mass)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return setBodySettings(shapeSettings.Create(), pose, mass);
        //auto bodyId = createEntity(ogreNode, bodyId);
    }

    World::Entity PhysicsSystemManager::createBoxEntity(Ogre::SceneNode* ogreNode, JPH::BodyID bodyId, const Types::Pose& pose, const JPH::RVec3& halfSize, float mass)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return createEntity(ogreNode, bodyId);
    }



    JPH::BodyID PhysicsSystemManager::createPhysSphere(const Types::Pose& pose, float radius, float mass)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return setBodySettings(shapeSettings.Create(), pose, mass);
        //auto bodyId = createEntity(ogreNode, bodyId);
    }

    World::Entity PhysicsSystemManager::createSphereEntity(Ogre::SceneNode* ogreNode, JPH::BodyID bodyId, const Types::Pose& pose, float radius, float mass)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return createEntity(ogreNode, bodyId);
    }

    World::Entity PhysicsSystemManager::createEntity(Ogre::SceneNode* ogreNode, JPH::BodyID bodyId)
    {
        World::Entity entity
        {
            .physId = bodyId,
#ifdef MAXDOF_CLIENT_BUILD
            .graphNode = ogreNode
#endif
        };

        mEntities.push_back(entity);
        mEntitiesIndex[bodyId.GetIndex()] = mEntities.size() - 1;

        return entity;
    }
}  // namespace MaxDoF::World
