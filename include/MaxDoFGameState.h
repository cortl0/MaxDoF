// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_TutorialGameState_H_
#define MaxDoF_TutorialGameState_H_

#include <OgrePrerequisites.h>
#include <GameState.h>
#include <TextOverleyManager.h>

#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/SpacecraftInputTranslator.h"
#include "SpacecraftController.h"
#include "World/Entity.h"
#include "World/Manager.h"


// Maximum Degrees of Freedom
namespace MaxDoF
{
    class MaxDoFGameState : public Demo::GameState
    {
    protected:
        Demo::GraphicsSystem *mGraphicsSystem{ nullptr };
        TextOverleyManager mTextOverleyManager;
        const unsigned int mMaxBodiesLimit{ 2048 };
        World::Entity mShip;
        World::Manager mWorldManager;
        std::unique_ptr<SpacecraftController> mSpacecraftController;
        Input::SpacecraftInputTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;

    public:
        MaxDoFGameState( const Ogre::String &helpDescription );
        ~MaxDoFGameState() override;
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void createCube();
        void createLight();
        void createShip();
        void createSphere();
        void createScene01() override;
        void update( float timeSinceLast ) override;
        void keyPressed( const SDL_KeyboardEvent &arg ) override;
        void keyReleased( const SDL_KeyboardEvent &arg ) override;
        void mouseMoved( const SDL_Event &arg ) override;
    };
}  // namespace MaxDoF

#endif
