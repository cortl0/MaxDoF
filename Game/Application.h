// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <memory>
#include <OgreString.h>
#include <OgreTimer.h>

#include "Game/MaxDoFGraphicsSystem.h"
#include "Game/MdfGame.h"

namespace MaxDoF::Game
{
    class Application final
    {
        std::unique_ptr<MaxDoFGraphicsSystem> mGraphicsSystem;
        std::unique_ptr<MdfGame> mGame;
        Ogre::Timer mTimer;
        bool mQuit{ false };

        void initialize( const Ogre::String &windowTitle, const Ogre::String &helpDescription );
        void shutdown();
        void processEvents();

    public:
        Application() = default;
        ~Application();

        void run( const Ogre::String &windowTitle, const Ogre::String &helpDescription );
    };
}
