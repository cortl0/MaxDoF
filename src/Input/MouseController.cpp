// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "Input/MouseController.h"

namespace MaxDoF::Input
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
    bool MouseController::isRightButtonPressed() const
    {
        int mouseX, mouseY;
        uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        return (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    }
}  // namespace MaxDoF::Input
