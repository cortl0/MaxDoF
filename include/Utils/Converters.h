// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_Utils_Converters_H_
#define MaxDoF_Utils_Converters_H_

#include <Jolt/Jolt.h>
#include <OgrePrerequisites.h>
#include "Utils/Types.h"

namespace MaxDoF::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& );
    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& );
    Ogre::Quaternion JphQuatToOgreQuaternion( const JPH::Quat& );
    Ogre::Vector3 JphRVec3ToOgreVector3( const JPH::RVec3& );
    Types::GraphPose PhysPoseToGraphPose(const Types::PhysPose& );
    Types::PhysPose GraphPoseToPhysPose(const Types::GraphPose& );
}  // namespace MaxDoF::Utils

#endif
