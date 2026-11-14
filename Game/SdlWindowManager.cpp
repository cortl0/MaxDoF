// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "SdlWindowManager.h"

#include <SDL_syswm.h>
#include <OgreException.h>
#include <OgreStringConverter.h>

namespace MaxDoF::Game
{
    SdlWindowManager::~SdlWindowManager()
    {
        destroyWindow();
    }

    void SdlWindowManager::createWindow( const Ogre::String &title, int width, int height, bool fullscreen )
    {
        if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS ) != 0 )
        {
            OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR,
                         "Cannot initialize SDL2: " + Ogre::String( SDL_GetError() ),
                         "SdlWindowManager::createWindow" );
        }

        uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
        if( fullscreen )
            flags |= SDL_WINDOW_FULLSCREEN;

        mWindow = SDL_CreateWindow( title.c_str(),
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    width, height, flags );
        if( !mWindow )
        {
            OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR,
                         "Failed to create SDL window: " + Ogre::String( SDL_GetError() ),
                         "SdlWindowManager::createWindow" );
        }
    }

    void SdlWindowManager::destroyWindow()
    {
        if( mWindow )
        {
            SDL_DestroyWindow( mWindow );
            mWindow = nullptr;
            SDL_Quit();
        }
    }

    Ogre::String SdlWindowManager::getExternalWindowHandle() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );
        if( !SDL_GetWindowWMInfo( mWindow, &wmInfo ) )
        {
            OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR,
                         "Failed to get SDL window WM info.",
                         "SdlWindowManager::getExternalWindowHandle" );
        }

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        return Ogre::StringConverter::toString( reinterpret_cast<size_t>( wmInfo.info.win.window ) );
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        return Ogre::StringConverter::toString( static_cast<size_t>( wmInfo.info.x11.window ) );
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        return Ogre::StringConverter::toString( reinterpret_cast<size_t>( wmInfo.info.cocoa.window ) );
#else
        return "";
#endif
    }

    void SdlWindowManager::setRelativeMouseMode( bool enabled )
    {
        if( mWindow )
        {
            SDL_RaiseWindow( mWindow );
            SDL_SetWindowGrab( mWindow, enabled ? SDL_TRUE : SDL_FALSE );
        }
        SDL_SetRelativeMouseMode( enabled ? SDL_TRUE : SDL_FALSE );
    }
}

