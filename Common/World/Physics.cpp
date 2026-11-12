// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/Physics.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::World
{
    // 1. Заглушка для слоев широкой фазы (BroadPhase)
    class MyBroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
    {
    public:
        virtual JPH::uint GetNumBroadPhaseLayers() const override { return 1; }
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return JPH::BroadPhaseLayer(0); }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override { return "Default"; }
#endif
    };

    // 2. Фильтр: Объект против Слоя широкой фазы (всегда разрешаем коллизию)
    class MyObjectVsBroadPhaseLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override { return true; }
    };

    // 3. Фильтр: Пара Объектов друг с другом (всегда разрешаем коллизию)
    class MyObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override { return true; }
    };

    Physics::Physics()
    {
    }

    Physics::~Physics()
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

    void Physics::destroyBody(JPH::BodyID bodyID)
    {
        if (!mBodyInterface->IsAdded(bodyID))
            return;

        if (mBodyInterface->IsActive(bodyID))
            mBodyInterface->DeactivateBody(bodyID);

        mBodyInterface->RemoveBody(bodyID);
    }

    void Physics::initialize(unsigned int maxBodies)
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
    }

    JPH::BodyID Physics::setBodySettings(Common::Types3d::EntityType userData, const JPH::ShapeSettings::ShapeResult &shapeResult, const Types::PhysPose &pose, float mass, bool dynamic, bool onCCD)
    {
        JPH::BodyCreationSettings bodySettings(
            shapeResult.Get(),
            pose.position,
            pose.orientation,
            (dynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static),
            JPH::ObjectLayer(0)
        );

        bodySettings.mAllowSleeping = false;
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = mass;
        bodySettings.mLinearDamping = 0.0f;
        bodySettings.mAngularDamping = 0.0f;
        bodySettings.mUserData = static_cast<JPH::uint64>(userData);

        if(onCCD)
            bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createBox(Common::Types3d::EntityType userData, const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool onCCD)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        shapeSettings.mUserData = static_cast<JPH::uint64>(userData);
        return setBodySettings(userData, shapeSettings.Create(), pose, mass, dynamic, onCCD);
    }

    JPH::BodyCreationSettings CreateMeshBodySettings(JPH::ShapeRefC mapShape)
    {
        JPH::BodyCreationSettings settings(
            mapShape,
            JPH::RVec3::sZero(),
            JPH::Quat::sIdentity(),
            JPH::EMotionType::Static,
            JPH::ObjectLayer(0)
        );

        return settings;
    }

    JPH::BodyID Physics::createMesh(const Types3d::MeshData &meshData)
    {
        if (meshData.vertices.empty() || meshData.indices.empty())
        {
            MaxDoF_LOG_AND_ABORT("bad data")
        }

        JPH::VertexList joltVertices;
        joltVertices.reserve(meshData.vertices.size());
        for (const auto& v : meshData.vertices)
        {
            joltVertices.push_back(JPH::Float3(v.x, v.y, v.z));
        }

        JPH::IndexedTriangleList joltTriangles;
        joltTriangles.reserve(meshData.indices.size() / 3);
        for (size_t i = 0; i < meshData.indices.size(); i += 3)
        {
            if (i + 2 < meshData.indices.size())
            {
                joltTriangles.push_back(JPH::IndexedTriangle(
                    meshData.indices[i],
                    meshData.indices[i + 1],
                    meshData.indices[i + 2]
                ));
            }
            else
            {
                MaxDoF_LOG_AND_ABORT("bad data")
            }
        }

        // Шаг 3: Сборка геометрии
        JPH::MeshShapeSettings meshSettings(joltVertices, joltTriangles);
        JPH::Shape::ShapeResult shapeResult = meshSettings.Create();
        JPH::ShapeRefC mapShape = shapeResult.Get();
        auto bodySettings = CreateMeshBodySettings(mapShape);
        bodySettings.mUserData = static_cast<JPH::uint64>(Common::Types3d::EntityType::cAsteroid);

        // Возвращаем ID тела статического уровня для контроля памяти (очистки при смене карты)
        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createSphere(Common::Types3d::EntityType userData, const Types::PhysPose& pose, float radius, float mass, bool onCCD)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        shapeSettings.mUserData = static_cast<JPH::uint64>(userData);
        return setBodySettings(userData, shapeSettings.Create(), pose, mass, true, onCCD);
    }

    void Physics::update(float timeSinceLast)
    {
        mPhysicsSystem->Update(timeSinceLast, 1, mPhysicsAllocator, mJobSystem);
    }
}
