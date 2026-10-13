// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_MeshProcedures_H_
#define MaxDoF_MeshProcedures_H_

#include <OgrePrerequisites.h>

namespace MaxDoF::Utils
{
    void createProceduralCubeV2( Ogre::SceneManager*, const Ogre::String &meshName, float halfX, float halfY, float halfZ );
    void createProceduralSphereV2( Ogre::SceneManager*, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments );
}  // namespace MaxDoF::Utils

#endif
