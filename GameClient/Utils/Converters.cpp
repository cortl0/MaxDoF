// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Utils/Converters.h"

namespace MaxDoF::GameClient::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& q )
    {
        return JPH::Quat( q.x, q.y, q.z, q.w );
    }

    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& v )
    {
        return JPH::RVec3( v.x, v.y, v.z );
    }

    Ogre::Quaternion JphQuatToOgreQuaternion( const JPH::Quat& q )
    {
        return Ogre::Quaternion( q.GetX(), q.GetY(), q.GetZ(), q.GetW() );
    }

    Ogre::Vector3 JphVec3ToOgreVector3( const JPH::Vec3& v )
    {
        return Ogre::Vector3( v.GetX(), v.GetY(), v.GetZ() );
    }

    Ogre::Vector3 JphRVec3ToOgreVector3( const JPH::RVec3& v )
    {
        return Ogre::Vector3( v.GetX(), v.GetY(), v.GetZ() );
    }

    GameCommon::Types::GraphPose PhysPoseToGraphPose(const GameCommon::Types::PhysPose& p)
    {
        return GameCommon::Types::GraphPose
        {
            .position = JphRVec3ToOgreVector3( p.position ),
            .orientation = JphQuatToOgreQuaternion( p.orientation )
        };
    }

    GameCommon::Types::PhysPose GraphPoseToPhysPose(const GameCommon::Types::GraphPose& p)
    {
        return GameCommon::Types::PhysPose
        {
            .position = OgreVector3ToJphRVec3( p.position ),
            .orientation = OgreQuaternionToJphQuat( p.orientation )
        };
    }
}
