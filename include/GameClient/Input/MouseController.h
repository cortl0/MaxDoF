// https://Github/cortl0/MaxDoF
// GPL-3.0 license

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
}  // namespace MaxDoF::Client::Input

#endif
