// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

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
            outData.leftMouseButtonPressed = mouse->isLeftButtonPressed();
            outData.rightMouseButtonPressed = mouse->isRightButtonPressed();
        }

        return outData;
    }
}  // namespace MaxDoF::GameClient::Input
