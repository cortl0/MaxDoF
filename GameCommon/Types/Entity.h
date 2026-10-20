// https://Github/cortl0/MaxDoF
// GPL-3.0 license

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
