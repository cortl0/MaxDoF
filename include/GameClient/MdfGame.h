// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameClient_Game_H_
#define MaxDoF_GameClient_Game_H_

#include <OgrePrerequisites.h>
#include <GameState.h>

#include "GameClient/TextOverleyManager.h"
#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/ControllerTranslator.h"
#include "Scenes/Scene01.h"
#include "ShipController.h"
#include "Types/Entity.h"
#include "World/Manager.h"

// Maximum Degrees of Freedom
namespace MaxDoF::GameClient
{
    class MdfGame : public Demo::GameState
    {
    protected:
        Demo::GraphicsSystem *mGraphicsSystem{ nullptr };
        TextOverleyManager mTextOverleyManager;
        const unsigned int mMaxBodiesLimit{ 2048 };
        Types::Entity mShip;
        MaxDoF::World::Manager mWorldManager;
        std::unique_ptr<ShipController> mSpacecraftController;
        Input::ControllerTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;
        std::unique_ptr<Scenes::Scene> mScene;

    public:
        MdfGame( const Ogre::String &helpDescription );
        ~MdfGame() override;
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void connectCameraAndControllerToShip();
        void createScene01() override;
        void update( float timeSinceLast ) override;
        void keyPressed( const SDL_KeyboardEvent &arg ) override;
        void keyReleased( const SDL_KeyboardEvent &arg ) override;
        void mouseMoved( const SDL_Event &arg ) override;
    };
}  // namespace MaxDoF::Client

#endif
