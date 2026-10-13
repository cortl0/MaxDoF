// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "Utils/PhysicsSystemManager.h"

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

#include "OgreSceneNode.h"
#include <algorithm>
#include <thread>

namespace MaxDoF
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

    JPH::BodyID PhysicsSystemManager::createShipBody(float radius, float massInKg)
    {
        JPH::SphereShapeSettings sphereSettings(radius);
        JPH::ShapeSettings::ShapeResult shapeResult = sphereSettings.Create();
        JPH::ShapeRefC shipShape = shapeResult.Get();

        JPH::BodyCreationSettings bodySettings(
            shipShape,
            JPH::RVec3::sZero(),
            JPH::Quat::sIdentity(),
            JPH::EMotionType::Dynamic,
            JPH::ObjectLayer(0)
        );

        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = massInKg;

        // Чистая инерция — убираем любое сопротивление сред
        bodySettings.mLinearDamping = 0.0f;
        bodySettings.mAngularDamping = 0.0f;

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    void PhysicsSystemManager::update(float timeSinceLast)
    {
        if (!mPhysicsSystem) return;

        float physicsTimeStep = std::min(timeSinceLast, 0.1f);
        mPhysicsSystem->Update(physicsTimeStep, 1, mPhysicsAllocator, mJobSystem);

        // АВТОМАТИЧЕСКАЯ СИНХРОНИЗАЦИЯ ВСЕХ ОБЪЕКТОВ СЦЕНЫ ИЗ ВЕКТОРА
        for (const auto& entity : mEntities)
        {
            if (!entity.graphNode) continue;

            JPH::RVec3 position;
            JPH::Quat rotation;
            mBodyInterface->GetPositionAndRotation(entity.physId, position, rotation);

            entity.graphNode->setPosition(position.GetX(), position.GetY(), position.GetZ());
            entity.graphNode->setOrientation(Ogre::Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()));
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

    World::Entity PhysicsSystemManager::createPhysBox(Ogre::SceneNode* ogreNode, const Ogre::Vector3& size, float massInKg)
    {
        // 1. Jolt Physics требует полуразмеры (half-extents) — расстояние от центра до грани
        JPH::Vec3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
        
        // 2. Создаем настройки формы коробки
        JPH::BoxShapeSettings boxSettings(halfExtents);
        JPH::ShapeRefC boxShape = boxSettings.Create().Get();

        // 3. Считываем начальную позицию и вращение из узла Ogre-Next
        Ogre::Vector3 ogrePos = ogreNode->getPosition();
        JPH::RVec3 startPosition(ogrePos.x, ogrePos.y, ogrePos.z);
        
        Ogre::Quaternion ogreQuat = ogreNode->getOrientation();
        JPH::Quat startRotation(ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w);

        // 4. Настраиваем физическое динамическое тело
        JPH::BodyCreationSettings bodySettings(
            boxShape, startPosition, startRotation, 
            JPH::EMotionType::Dynamic, JPH::ObjectLayer(0)
        );
        
        // Jolt автоматически посчитает тензор инерции для куба, чтобы он правильно крутился при ударах
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = massInKg;
        bodySettings.mLinearDamping = 0.0f;
        bodySettings.mAngularDamping = 0.0f;

        // 5. Создаем тело в физическом мире
        JPH::BodyID bodyId = mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

        return createEntity(ogreNode, bodyId);
    }

    World::Entity PhysicsSystemManager::createPhysSphere(Ogre::SceneNode* ogreNode, float radius, float massInKg)
    {
        JPH::SphereShapeSettings sphereSettings(radius);
        JPH::ShapeRefC shipShape = sphereSettings.Create().Get();

        JPH::RVec3 startPosition(ogreNode->getPosition().x, ogreNode->getPosition().y, ogreNode->getPosition().z);

        // Правильный порядок кватерниона из Ogre в Jolt (x, y, z, w)
        Ogre::Quaternion ogreQuat = ogreNode->getOrientation();
        JPH::Quat startRotation(ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w);

        JPH::BodyCreationSettings bodySettings(
            shipShape, startPosition, startRotation,
            JPH::EMotionType::Dynamic, JPH::ObjectLayer(0)
        );

        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = massInKg;
        bodySettings.mLinearDamping = 0.0f;
        bodySettings.mAngularDamping = 0.0f;

        JPH::BodyID bodyId = mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

        return createEntity(ogreNode, bodyId);
    }

    World::Entity PhysicsSystemManager::createEntity(Ogre::SceneNode* ogreNode, JPH::BodyID bodyId)
    {
        World::Entity entity
        {
            .physId = bodyId,
            .graphNode = ogreNode
        };

        mEntities.push_back(entity);
        mEntitiesIndex[bodyId.GetIndex()] = mEntities.size() - 1;

        return entity;
    }
}  // namespace MaxDoF
