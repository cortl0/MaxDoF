// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_GameServer_MdfServer_H_
#define MaxDoF_GameServer_MdfServer_H_

#include "GameCommon/Scenes/Scene01.h"
#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameServer
{
    class MdfServer
    {
        GameCommon::World::Manager mWorldManager;
        std::unique_ptr<GameCommon::Scenes::Scene> mScene;

    public:
        MdfServer();
        ~MdfServer();
        void createScene01();
    };
}  // namespace MaxDoF::GameServer

#endif
