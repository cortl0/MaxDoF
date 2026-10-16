// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameCommon_Types_Poses_H_
#define MaxDoF_GameCommon_Types_Poses_H_

#include <Jolt/Jolt.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#endif

namespace MaxDoF::GameCommon::Types
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
}  // namespace MaxDoF::GameCommon::Types

#endif
