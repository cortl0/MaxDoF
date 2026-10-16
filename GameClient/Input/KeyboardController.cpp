// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Input/KeyboardController.h"

namespace MaxDoF::GameClient::Input
{
    KeyboardController::KeyboardController() :
        mSpeedModifier( false )
    {
        memset( mWASD, 0, sizeof( mWASD ) );
        memset( mSlideUpDown, 0, sizeof( mSlideUpDown ) );
    }

    void KeyboardController::reset()
    {
        mSpeedModifier = false;
        memset( mWASD, 0, sizeof( mWASD ) );
        memset( mSlideUpDown, 0, sizeof( mSlideUpDown ) );
    }

    bool KeyboardController::keyPressed( const SDL_KeyboardEvent &arg )
    {
        /*if( arg.keysym.scancode == SDL_SCANCODE_LSHIFT )
            mSpeedModifier = true;
        else*/
        if( arg.keysym.scancode == SDL_SCANCODE_W )
            mWASD[0] = true;
        else if( arg.keysym.scancode == SDL_SCANCODE_A )
            mWASD[1] = true;
        else if( arg.keysym.scancode == SDL_SCANCODE_S )
            mWASD[2] = true;
        else if( arg.keysym.scancode == SDL_SCANCODE_D )
            mWASD[3] = true;
        else if( arg.keysym.scancode == SDL_SCANCODE_SPACE )
            mSlideUpDown[0] = true;
        else if( arg.keysym.scancode == SDL_SCANCODE_LSHIFT )
            mSlideUpDown[1] = true;
        else
            return false;

        return true;
    }

    bool KeyboardController::keyReleased( const SDL_KeyboardEvent &arg )
    {
        /*if( arg.keysym.scancode == SDL_SCANCODE_LSHIFT )
            mSpeedModifier = false;
        else*/
        if( arg.keysym.scancode == SDL_SCANCODE_W )
            mWASD[0] = false;
        else if( arg.keysym.scancode == SDL_SCANCODE_A )
            mWASD[1] = false;
        else if( arg.keysym.scancode == SDL_SCANCODE_S )
            mWASD[2] = false;
        else if( arg.keysym.scancode == SDL_SCANCODE_D )
            mWASD[3] = false;
        else if( arg.keysym.scancode == SDL_SCANCODE_SPACE )
            mSlideUpDown[0] = false;
        else if( arg.keysym.scancode == SDL_SCANCODE_LSHIFT )
            mSlideUpDown[1] = false;
        else
            return false;

        return true;
    }

    Ogre::Vector3 KeyboardController::getMovementDirection() const
    {
        int camMovementZ = mWASD[2] - mWASD[0]; // S - W
        int camMovementX = mWASD[3] - mWASD[1]; // D - A
        int slideUpDown  = mSlideUpDown[0] - mSlideUpDown[1]; // PageUp - PageDown

        return Ogre::Vector3( (Ogre::Real)camMovementX, (Ogre::Real)slideUpDown, (Ogre::Real)camMovementZ );
    }

    KeyboardController::KeyboardRawState KeyboardController::getRawState() const
    {
        KeyboardRawState state;
        state.w        = mWASD[0];
        state.a        = mWASD[1];
        state.s        = mWASD[2];
        state.d        = mWASD[3];
        state.pageUp   = mSlideUpDown[0];
        state.pageDown = mSlideUpDown[1];
        state.shift    = mSpeedModifier;

        return state;
    }
}  // namespace MaxDoF::Client::Input
