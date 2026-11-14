// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <SDL.h>
#include <OgrePrerequisites.h>

namespace MaxDoF::Game::Core
{
    class SdlWindowManager final
    {
        SDL_Window *mWindow{ nullptr };

    public:
        SdlWindowManager() = default;
        ~SdlWindowManager();

        void createWindow( const Ogre::String &title, int width, int height, bool fullscreen );
        void destroyWindow();

        Ogre::String getExternalWindowHandle() const;
        void setRelativeMouseMode( bool enabled );
        SDL_Window *getSdlWindow() const { return mWindow; }
    };
}
