// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Scenes/Scene.h"

namespace MaxDoF::Common::Scenes
{
    class SceneTest : public Scene
    {
        static constexpr double mapHalfSize{75.0f};

        void createBoxes(JPH::RVec3 offset);

#ifdef MAXDOF_CLIENT_BUILD
        void createStaticLights();
#endif

        void createMapBoundaries();

    public:
        SceneTest(World::Manager&);
        ~SceneTest() override;
        void CreateScene() override;
    };
}
