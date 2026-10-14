// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_World_Graphics_H_
#define MaxDoF_World_Graphics_H_

#include <GraphicsSystem.h>
#include <OgrePrerequisites.h>

#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/SpacecraftInputTranslator.h"
#include "SpacecraftController.h"
#include "TextOverleyManager.h"
#include "World/Entity.h"
#include "Utils/Types.h"

namespace Ogre
{
    namespace v1
    {
        class TextAreaOverlayElement;
    }
}  // namespace Ogre

// Maximum Degrees of Freedom
namespace MaxDoF::World
{
    class Graphics
    {
    protected:
        TextOverleyManager mTextOverleyManager;
        World::Entity mShip;
        Input::SpacecraftInputTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;

    public:
        const Ogre::String ortSphereMeshName{"MyV2Sphere"};
        const Ogre::String ortCubeMeshName{"MyV2Cube"};
        Demo::GraphicsSystem *mGraphicsSystem;

        Graphics( const Ogre::String &helpDescription );
        ~Graphics();
        Ogre::SceneNode* createCube();
        Ogre::SceneNode* createLight();
        Ogre::SceneNode* createGraphShape( const Ogre::String& meshName, const Types::GraphPose& pose = Types::cDefaultGraphPose);
        Ogre::SceneNode* createSphere();
    };
}  // namespace MaxDoF::World

#endif
