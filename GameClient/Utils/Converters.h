// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <Jolt/Jolt.h>
#include <OgrePrerequisites.h>

#include "GameCommon/Types/Poses.h"

namespace MaxDoF::GameClient::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& );
    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& );
    Ogre::Quaternion JphQuatToOgreQuaternion( const JPH::Quat& );
    Ogre::Vector3 JphVec3ToOgreVector3( const JPH::Vec3& );
    Ogre::Vector3 JphRVec3ToOgreVector3( const JPH::RVec3& );
    GameCommon::Types::GraphPose PhysPoseToGraphPose(const GameCommon::Types::PhysPose& );
    GameCommon::Types::PhysPose GraphPoseToPhysPose(const GameCommon::Types::GraphPose& );
}
