// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <SDL.h>
#include <OgrePrerequisites.h>
#include <OgreString.h>

#include "Game/Core/MaxDoFGraphicsSystem.h"
#include "Game/UI/TextOverleyManager.h"
#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/ControllerTranslator.h"
#include "Common/GameLogic/BattleLogic.h"
#include "Game/Ships/ShipController.h"
#include "Common/Scenes/Scene01.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Game
{
    class MdfGame final
    {
        JPH::Quat qCameraPreviousQuaternion{ JPH::Quat::sIdentity() };
        JPH::Vec3 getJoltLocalAngularVelocities( JPH::Quat &qPrev, const JPH::Quat &qCurr, float dt );

        void connectControllerToShip();
        void keyPressed( const SDL_KeyboardEvent &arg );
        void keyReleased( const SDL_KeyboardEvent &arg );
        void mouseMoved( const SDL_Event &arg );

    protected:
        Core::MaxDoFGraphicsSystem *mGraphicsSystem{ nullptr };
        UI::TextOverleyManager mTextOverleyManager;
        const unsigned int mMaxBodiesLimit{ 2048 };
        Common::World::Manager mWorldManager;
        std::unique_ptr<Ships::ShipController> mShipController;
        Input::ControllerTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;
        std::unique_ptr<Common::Scenes::Scene> mScene;
        Common::GameLogic::BattleLogic mBattleLogic{ mWorldManager };

    public:
        explicit MdfGame( const Ogre::String &helpDescription );
        ~MdfGame();

        void _notifyGraphicsSystem( Core::MaxDoFGraphicsSystem *graphicsSystem );
        void createScene01();
        void update( float timeSinceLast );
        void handleEvent( const SDL_Event &evt );
    };
}
