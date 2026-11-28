// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgreVector3.h>

namespace MaxDoF::Game::Ships
{
    struct ControllerData
    {
        float mouse_dx{0.0f};
        float mouse_dy{0.0f};
        bool leftMouseButtonPressed{false};
        bool rightMouseButtonPressed{false};
        Ogre::Vector3 moveDir{Ogre::Vector3::ZERO};
    };
}
