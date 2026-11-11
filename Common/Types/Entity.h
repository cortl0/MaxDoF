// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreSceneNode.h>
#endif

namespace MaxDoF::GameCommon::Types
{
    struct Entity
    {
        Entity(const JPH::BodyID& physId
#ifdef MAXDOF_CLIENT_BUILD
            , Ogre::SceneNode* graphNode
#endif
        ) : physId(physId)
#ifdef MAXDOF_CLIENT_BUILD
        , graphNode(graphNode)
#endif
        {}
        JPH::BodyID physId;
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* graphNode{ nullptr };
#endif
    };
}
