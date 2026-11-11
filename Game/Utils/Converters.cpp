// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Game/Utils/Converters.h"

namespace MaxDoF::Game::Utils
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

    Common::Types::GraphPose PhysPoseToGraphPose(const Common::Types::PhysPose& p)
    {
        return Common::Types::GraphPose
        {
            .position = JphRVec3ToOgreVector3( p.position ),
            .orientation = JphQuatToOgreQuaternion( p.orientation )
        };
    }

    Common::Types::PhysPose GraphPoseToPhysPose(const Common::Types::GraphPose& p)
    {
        return Common::Types::PhysPose
        {
            .position = OgreVector3ToJphRVec3( p.position ),
            .orientation = OgreQuaternionToJphQuat( p.orientation )
        };
    }
}
