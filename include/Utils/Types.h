// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Types_Converters_H_
#define MaxDoF_Types_Converters_H_

#ifndef MAXDOF_CLIENT_BUILD
#define MAXDOF_CLIENT_BUILD
#endif

#include <Jolt/Jolt.h>

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgrePrerequisites.h>
#endif

namespace MaxDoF::Types
{
    inline const JPH::RVec3 cUnitScaleVector{ 1.0f, 1.0f, 1.0f };
    inline const JPH::RVec3 cZeroVector = JPH::RVec3::sZero();
    inline const JPH::Quat cIdentity = JPH::Quat::sIdentity();

    struct PhysPose
    {
        JPH::RVec3 position;
        JPH::Quat orientation;
    };

    inline const PhysPose cDefaultPhysPose
    {
        .position = cZeroVector,
        .orientation = cIdentity
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
}  // namespace MaxDoF::Types

#endif
