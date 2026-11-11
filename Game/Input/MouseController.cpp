// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Game/Input/MouseController.h"

namespace MaxDoF::Game::Input
{
    MouseController::MouseController() :
        mRelativeX( 0.0f ),
        mRelativeY( 0.0f )
    {
    }

    void MouseController::mouseMoved( const SDL_Event &arg )
    {
        mRelativeX += static_cast<float>( arg.motion.xrel );
        mRelativeY += static_cast<float>( arg.motion.yrel );
    }

    void MouseController::reset()
    {
        mRelativeX = 0.0f;
        mRelativeY = 0.0f;
    }

    bool MouseController::isLeftButtonPressed() const
    {
        int mouseX, mouseY;
        uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        return (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    }

    bool MouseController::isRightButtonPressed() const
    {
        int mouseX, mouseY;
        uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        return (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    }
}  // namespace MaxDoF::Game::Input
