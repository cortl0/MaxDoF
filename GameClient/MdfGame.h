// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <OgrePrerequisites.h>
#include <GameState.h>

#include "GameClient/TextOverleyManager.h"
#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/ControllerTranslator.h"
#include "GameClient/Ships/Ship.h"
#include "GameClient/ShipController.h"
#include "GameCommon/Scenes/Scene01.h"
#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameClient
{
    class MdfGame : public Demo::GameState
    {
        JPH::Quat qCameraPreviousQuaternion{JPH::Quat::sIdentity()};
        JPH::Vec3 getJoltLocalAngularVelocities(JPH::Quat &qPrev, const JPH::Quat &qCurr, float dt);

        protected:
        Demo::GraphicsSystem *mGraphicsSystem{ nullptr };
        TextOverleyManager mTextOverleyManager;
        const unsigned int mMaxBodiesLimit{ 2048 };
        GameCommon::World::Manager mWorldManager;
        std::unique_ptr<ShipController> mShipController;
        Input::ControllerTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;
        std::unique_ptr<GameCommon::Scenes::Scene> mScene;
        GameClient::Ships::Ship* mShip;

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
}
