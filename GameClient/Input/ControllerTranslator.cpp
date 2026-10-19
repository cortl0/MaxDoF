// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Input/ControllerTranslator.h"

#include <OgreVector3.h>

#include "GameClient/Input/KeyboardController.h"
#include "GameClient/Input/MouseController.h"

namespace MaxDoF::GameClient::Input
{
    ShipController::ControllerData ControllerTranslator::translate(
        const KeyboardController* keyboard,
        const MouseController* mouse)
    {
        ShipController::ControllerData outData;

        KeyboardController::KeyboardRawState keys = keyboard->getRawState();

        int moveZ = keys.s - keys.w;
        int moveX = keys.d - keys.a;
        int moveY = keys.pageUp - keys.pageDown;

        outData.moveDir = Ogre::Vector3( static_cast<float>(moveX),
                                        static_cast<float>(moveY),
                                        static_cast<float>(moveZ) );

        if( mouse )
        {
            outData.mouse_dx = mouse->getRelativeX();
            outData.mouse_dy = mouse->getRelativeY();
            outData.rightMouseButtonPressed = mouse->isRightButtonPressed();
        }

        return outData;
    }
}  // namespace MaxDoF::GameClient::Input
