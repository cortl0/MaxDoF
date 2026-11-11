// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/Physics.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::World
{
    Physics::Physics()
    {
    }

    Physics::~Physics()
    {
    }

    void Physics::destroyBody(JPH::BodyID bodyID)
    {
        if (!mBodyInterface->IsAdded(bodyID))
            return;

        if (mBodyInterface->IsActive(bodyID))
            mBodyInterface->DeactivateBody(bodyID);

        mBodyInterface->RemoveBody(bodyID);
    }

    JPH::BodyID Physics::setBodySettings(JPH::uint64 userData, const JPH::ShapeSettings::ShapeResult& shapeResult, const Types::PhysPose& pose, float mass, bool dynamic, bool onCCD)
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
        bodySettings.mUserData = userData;

        if(onCCD)
            bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createBox(JPH::uint64 userData, const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool onCCD)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        shapeSettings.mUserData = userData + 12;
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

        // Возвращаем ID тела статического уровня для контроля памяти (очистки при смене карты)
        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createSphere(JPH::uint64 userData, const Types::PhysPose& pose, float radius, float mass, bool onCCD)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        shapeSettings.mUserData = userData + 15;
        return setBodySettings(userData, shapeSettings.Create(), pose, mass, true, onCCD);
    }
}
