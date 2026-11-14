// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "GraphicsSystem.h"

namespace MaxDoF::Game
{
    class MaxDoFGraphicsSystem final : public Demo::GraphicsSystem
    {
        Ogre::CompositorWorkspace *setupCompositor() override;
        void setupResources() override;

    public:
        explicit MaxDoFGraphicsSystem( Demo::GameState *gameState );
    };
}
