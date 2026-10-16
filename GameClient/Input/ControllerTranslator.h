// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameClient_Input_ControllerTranslator_H_
#define MaxDoF_GameClient_Input_ControllerTranslator_H_

#include "GameClient/ShipController.h"

namespace MaxDoF::GameClient::Input
{
    class KeyboardController;
    class MouseController;

    class ControllerTranslator
    {
    public:
        ShipController::ControllerData translate(
            const KeyboardController* keyboard,
            const MouseController* mouse
        );
    };
}  // namespace MaxDoF::GameClient::Input

#endif
