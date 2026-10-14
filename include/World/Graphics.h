// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_World_Graphics_H_
#define MaxDoF_World_Graphics_H_

#ifndef MAXDOF_CLIENT_BUILD
#define MAXDOF_CLIENT_BUILD
#endif

#include <GraphicsSystem.h>
#include <TextOverleyManager.h>

#include "Utils/Types.h"

namespace MaxDoF::World
{
    class Graphics
    {
    protected:
        TextOverleyManager mTextOverleyManager;
        Demo::GraphicsSystem *mGraphicsSystem;

    public:
        const Ogre::String ortSphereMeshName{"MyV2Sphere"};
        const Ogre::String ortCubeMeshName{"MyV2Cube"};

        Graphics( const Ogre::String &helpDescription );
        ~Graphics();

        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void initialize();
        Ogre::SceneNode* createCube(const Types::GraphPose&, const Ogre::Vector3& halfSize);
        Ogre::SceneNode* createLight();
        Ogre::SceneNode* createShape( const Ogre::String& meshName, const Types::GraphPose&);
        Ogre::SceneNode* createSphere(const Types::GraphPose&, float radius);
    };
}  // namespace MaxDoF::World

#endif
