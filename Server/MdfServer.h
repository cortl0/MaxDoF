// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Scenes/Scene.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Server
{
    class MdfServer
    {
        Common::World::Manager mWorldManager;
        std::unique_ptr<Common::Scenes::Scene> mScene;

    public:
        MdfServer();
        ~MdfServer();
        void createScene01();
    };
}
