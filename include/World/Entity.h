// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_Entity_H_
#define MaxDoF_Entity_H_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <OgreSceneNode.h>

namespace MaxDoF::World
{
    struct Entity
    {
        JPH::BodyID physId;
        Ogre::SceneNode* graphNode{ nullptr };
    };
}  // namespace MaxDoF::World

#endif
