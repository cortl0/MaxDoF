// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/World/Physics.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace MaxDoF::GameCommon::World
{
    Physics::Physics()
    {
    }

    Physics::~Physics()
    {
    }

    JPH::BodyID Physics::setBodySettings(const JPH::ShapeSettings::ShapeResult& shapeResult, const Types::PhysPose& pose, float mass, bool dynamic)
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

        return mBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    }

    JPH::BodyID Physics::createBox(const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic)
    {
        JPH::BoxShapeSettings shapeSettings( halfSize );
        return setBodySettings(shapeSettings.Create(), pose, mass, dynamic);
    }

    JPH::BodyID Physics::createSphere(const Types::PhysPose& pose, float radius, float mass)
    {
        JPH::SphereShapeSettings shapeSettings(radius);
        return setBodySettings(shapeSettings.Create(), pose, mass, true);
    }

}
