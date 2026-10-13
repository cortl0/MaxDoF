// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "Utils/Converters.h"

#include <OgreQuaternion.h>
#include <OgreVector3.h>

namespace MaxDoF::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& ogreQuat )
    {
        return JPH::Quat( ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w );
    }

    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& ogreVector )
    {
        return JPH::RVec3( ogreVector.x, ogreVector.y, ogreVector.z );
    }
}  // namespace MaxDoF::Utils
