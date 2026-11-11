// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <OgrePrerequisites.h>

#include "Common/Types/Poses.h"

namespace MaxDoF::Game::Utils
{
    JPH::Quat OgreQuaternionToJphQuat( const Ogre::Quaternion& );
    JPH::RVec3 OgreVector3ToJphRVec3( const Ogre::Vector3& );
    Ogre::Quaternion JphQuatToOgreQuaternion( const JPH::Quat& );
    Ogre::Vector3 JphVec3ToOgreVector3( const JPH::Vec3& );
    Ogre::Vector3 JphRVec3ToOgreVector3( const JPH::RVec3& );
    Common::Types::GraphPose PhysPoseToGraphPose(const Common::Types::PhysPose& );
    Common::Types::PhysPose GraphPoseToPhysPose(const Common::Types::GraphPose& );
}
