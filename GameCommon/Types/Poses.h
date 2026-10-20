// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <Jolt/Jolt.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#endif

namespace MaxDoF::GameCommon::Types
{
    inline const JPH::Vec3 cUnitScaleVector{ 1.0f, 1.0f, 1.0f };

    struct PhysPose
    {
        JPH::RVec3 position;
        JPH::Quat orientation;
    };

    inline const PhysPose cDefaultPhysPose
    {
        .position = JPH::RVec3::sZero(),
        .orientation = JPH::Quat::sIdentity()
    };

#ifdef MAXDOF_CLIENT_BUILD
    struct GraphPose
    {
        Ogre::Vector3 position;
        Ogre::Quaternion orientation;
    };

    inline const GraphPose cDefaultGraphPose
    {
        .position = Ogre::Vector3::ZERO,
        .orientation = Ogre::Quaternion::IDENTITY
    };
#endif
}
