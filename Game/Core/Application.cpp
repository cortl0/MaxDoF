// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Game/Core/Application.h"

#include <algorithm>
#include <SDL.h>
#include <OgreRoot.h>
#include <OgreWindow.h>

namespace MaxDoF::Game::Core
{
    Application::~Application()
    {
        shutdown();
    }

    void Application::initialize( const Ogre::String &windowTitle, const Ogre::String &helpDescription )
    {
        mGraphicsSystem = std::make_unique<MaxDoFGraphicsSystem>();
        mGame = std::make_unique<MdfGame>( helpDescription );

        mGame->_notifyGraphicsSystem( mGraphicsSystem.get() );
        mGraphicsSystem->initialize( windowTitle );
        mGame->createScene01();
    }

    void Application::shutdown()
    {
        mGame.reset();
        if( mGraphicsSystem )
        {
            mGraphicsSystem->deinitialize();
            mGraphicsSystem.reset();
        }
    }

    void Application::processEvents()
    {
        SDL_Event evt;
        while( SDL_PollEvent( &evt ) )
        {
            if( evt.type == SDL_QUIT ||
                ( evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) )
            {
                mQuit = true;
            }
            else if( mGame )
            {
                mGame->handleEvent( evt );
            }
        }
    }

    void Application::run( const Ogre::String &windowTitle, const Ogre::String &helpDescription )
    {
        initialize( windowTitle, helpDescription );

        mTimer.reset();
        uint64_t startTime = mTimer.getMicroseconds();
        double timeSinceLast = 1.0 / 60.0;

        while( !mQuit )
        {
            processEvents();

            if( mQuit || mGraphicsSystem->getRenderWindow()->isClosed() )
                break;

            mGame->update( static_cast<float>( timeSinceLast ) );

            if( !mGraphicsSystem->getRoot()->renderOneFrame() )
                break;

            const uint64_t endTime = mTimer.getMicroseconds();
            timeSinceLast = std::min( 1.0, ( endTime - startTime ) / 1000000.0 );
            startTime = endTime;
        }

        shutdown();
    }
}
