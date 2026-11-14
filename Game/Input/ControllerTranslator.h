// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Game/Ships/ShipController.h"

namespace MaxDoF::Game::Input
{
    class KeyboardController;
    class MouseController;

    class ControllerTranslator
    {
    public:
        Ships::ShipController::ControllerData translate(
            const KeyboardController* keyboard,
            const MouseController* mouse
        );
    };
}
