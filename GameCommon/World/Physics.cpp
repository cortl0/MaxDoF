// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/World/Physics.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

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

    JPH::BodyID Physics::createBox(const Types::PhysPose& pose, const JPH::RVec3& halfSize, float mass)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return setBodySettings(shapeSettings.Create(), pose, mass);
    }

    JPH::BodyID Physics::createSphere(const Types::PhysPose& pose, float radius, float mass)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return setBodySettings(shapeSettings.Create(), pose, mass);
    }

}  // namespace MaxDoF::World
