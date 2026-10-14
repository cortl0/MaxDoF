// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "World/Physics.h"

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
    Physics::Physics()
    {
    }

    Physics::~Physics()
    {
    }

    JPH::BodyID Physics::setBodySettings(const JPH::ShapeSettings::ShapeResult& shapeResult, const Types::PhysPose& pose, float mass)
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

    JPH::BodyID Physics::createPhysBox(const Types::PhysPose& pose, const JPH::RVec3& halfSize, float mass)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return setBodySettings(shapeSettings.Create(), pose, mass);
    }

    JPH::BodyID Physics::createPhysSphere(const Types::PhysPose& pose, float radius, float mass)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return setBodySettings(shapeSettings.Create(), pose, mass);
    }

}  // namespace MaxDoF::World
