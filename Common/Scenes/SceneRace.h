// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Scenes/Scene.h"

namespace MaxDoF::Common::Scenes
{
    static constexpr double mapHalfSize{150.0f};

    class SceneRace : public Scene
    {
    public:
        SceneRace(World::Manager&);
        ~SceneRace() override;
        void createStaticLights();
        void CreateScene() override;
    };
}
