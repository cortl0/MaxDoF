// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_MeshProcedures_H_
#define MaxDoF_MeshProcedures_H_

#include "OgreSceneManager.h"

namespace MaxDoF::Utils
{
    void createProceduralSphereV2(Ogre::SceneManager* sceneManager, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments);
}  // namespace MaxDoF::Utils

#endif
