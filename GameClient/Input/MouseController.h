// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_GameClient_Input_MouseController_H_
#define MaxDoF_GameClient_Input_MouseController_H_

#include <SDL_events.h>

namespace MaxDoF::GameClient::Input
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
        bool isRightButtonPressed() const;
    };
}  // namespace MaxDoF::GameClient::Input

#endif
