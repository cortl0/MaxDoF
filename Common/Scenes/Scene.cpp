// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Scenes/Scene.h"

namespace MaxDoF::Common::Scenes
{
    Scene::Scene(World::Manager& worldManager) :
        mWorldManager(worldManager)
    {
    }

    Scene::~Scene()
    {
    }
}
