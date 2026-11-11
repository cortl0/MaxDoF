// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgrePrerequisites.h>
#include <GameState.h>

#include "Game/TextOverleyManager.h"
#include "Input/MouseController.h"
#include "Input/KeyboardController.h"
#include "Input/ControllerTranslator.h"
#include "Common/GameLogic/BattleLogic.h"
#include "Game/ShipController.h"
#include "Common/Scenes/Scene01.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Game
{
    class MdfGame : public Demo::GameState
    {
        JPH::Quat qCameraPreviousQuaternion{JPH::Quat::sIdentity()};
        JPH::Vec3 getJoltLocalAngularVelocities(JPH::Quat &qPrev, const JPH::Quat &qCurr, float dt);

        void connectControllerToShip();

    protected:
        Demo::GraphicsSystem *mGraphicsSystem{ nullptr };
        TextOverleyManager mTextOverleyManager;
        const unsigned int mMaxBodiesLimit{ 2048 };
        Common::World::Manager mWorldManager;
        std::unique_ptr<ShipController> mShipController;
        Input::ControllerTranslator mInputTranslator;
        Input::MouseController mMouseController;
        Input::KeyboardController mKeyboardController;
        std::unique_ptr<Common::Scenes::Scene> mScene;
        Common::GameLogic::BattleLogic mBattleLogic{mWorldManager};

    public:
        MdfGame( const Ogre::String &helpDescription );
        ~MdfGame() override;
        void _notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem );
        void createScene01() override;
        void update( float timeSinceLast ) override;
        void keyPressed( const SDL_KeyboardEvent &arg ) override;
        void keyReleased( const SDL_KeyboardEvent &arg ) override;
        void mouseMoved( const SDL_Event &arg ) override;
    };
}
