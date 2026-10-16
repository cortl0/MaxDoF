// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Types_Entity_H_
#define MaxDoF_Types_Entity_H_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreSceneNode.h>
#endif

namespace MaxDoF::Types
{
    struct Entity
    {
        JPH::BodyID physId;
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* graphNode{ nullptr };
#endif
    };
}  // namespace MaxDoF::Types

#endif
