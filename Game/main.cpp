// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include <iostream>
#include <memory>

#include <SDL.h>
#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreTimer.h>
#include <OgreWindow.h>

#include "version.h"
#include "Game/MaxDoFGraphicsSystem.h"
#include "Game/MdfGame.h"

int main( int argc, char *argv[] )
{
    std::cout << "MaxDoF Game Version: " << MaxDoF_VERSION_STRING << std::endl;

    try
    {
        auto game = std::make_unique<MaxDoF::Game::MdfGame>( "MaxDoF Client" );
        auto graphicsSystem = std::make_unique<MaxDoF::Game::MaxDoFGraphicsSystem>();

        game->_notifyGraphicsSystem( graphicsSystem.get() );
        graphicsSystem->initialize( "MaxDoF" );
        game->createScene01();

        Ogre::Timer timer;
        uint64_t startTime = timer.getMicroseconds();
        double timeSinceLast = 1.0 / 60.0;

        bool quit = false;
        while( !quit )
        {
            SDL_Event evt;
            while( SDL_PollEvent( &evt ) )
            {
                if( evt.type == SDL_QUIT ||
                    ( evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) )
                {
                    quit = true;
                }
                else
                {
                    game->handleEvent( evt );
                }
            }

            if( quit || graphicsSystem->getRenderWindow()->isClosed() )
                break;

            game->update( static_cast<float>( timeSinceLast ) );

            if( !graphicsSystem->getRoot()->renderOneFrame() )
                break;

            uint64_t endTime = timer.getMicroseconds();
            timeSinceLast = ( endTime - startTime ) / 1000000.0;
            timeSinceLast = std::min( 1.0, timeSinceLast );
            startTime = endTime;
        }

        graphicsSystem->deinitialize();
    }
    catch( Ogre::Exception &e )
    {
        std::cerr << "An exception has occurred: " << e.getFullDescription() << std::endl;
        return 1;
    }

    return 0;
}
