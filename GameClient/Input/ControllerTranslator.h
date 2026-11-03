// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

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
