// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_Utils_Converters_H_
#define MaxDoF_Utils_Converters_H_

#include <Jolt/Jolt.h>
#include <OgrePrerequisites.h>

namespace MaxDoF::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& );
    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& );
}  // namespace MaxDoF::Utils

#endif
