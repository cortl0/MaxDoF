// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <SDL_events.h>

namespace MaxDoF::Game::Input
{
    class MouseController
    {
        float mRelativeX;
        float mRelativeY;

    public:
        MouseController();

        void mouseMoved( const SDL_Event &arg );
        void reset();

        float getRelativeX() const { return mRelativeX; }
        float getRelativeY() const { return mRelativeY; }
        bool isLeftButtonPressed() const;
        bool isRightButtonPressed() const;
    };
}
