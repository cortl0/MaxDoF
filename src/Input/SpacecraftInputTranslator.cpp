// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "Input/SpacecraftInputTranslator.h"
#include "Input/KeyboardController.h"
#include "Input/MouseController.h"
#include "OgreVector3.h"

namespace MaxDoF::Input
{
    SpacecraftController::ControllerData SpacecraftInputTranslator::translate(
        const KeyboardController* keyboard,
        const MouseController* mouse)
    {
        SpacecraftController::ControllerData outData;

        KeyboardController::KeyboardRawState keys = keyboard->getRawState();

        int moveZ = keys.s - keys.w;
        int moveX = keys.d - keys.a;
        int moveY = keys.pageUp - keys.pageDown;

        outData.moveDir = Ogre::Vector3( static_cast<float>(moveX),
                                        static_cast<float>(moveY),
                                        static_cast<float>(moveZ) );

        outData.speedBoost = keys.shift;

        if( mouse )
        {
            outData.mouse_dx = mouse->getRelativeX();
            outData.mouse_dy = mouse->getRelativeY();
            outData.rightMouseButtonPressed = mouse->isRightButtonPressed();
        }

        return outData;
    }
}  // namespace MaxDoF::Input
