// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "GameCommon/World/Physics.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "GameCommon/Utils/SystemUtils.h"

namespace MaxDoF::GameCommon::World
{
    Physics::Physics()
    {
    }

    Physics::~Physics()
    {
    }

    JPH::BodyID Physics::setBodySettings(const JPH::ShapeSettings::ShapeResult& shapeResult, const Types::PhysPose& pose, float mass, bool dynamic, bool оnCCD)
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

        if(оnCCD)
            bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createBox(const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool оnCCD)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return setBodySettings(shapeSettings.Create(), pose, mass, dynamic, оnCCD);
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

    JPH::BodyID Physics::createSphere(const Types::PhysPose& pose, float radius, float mass, bool оnCCD)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return setBodySettings(shapeSettings.Create(), pose, mass, true, оnCCD);
    }
}
