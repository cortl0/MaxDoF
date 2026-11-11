// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Server/MdfServer.h"

#include "Common/Scenes/Scene01.h"

namespace MaxDoF::Server
{
    MdfServer::MdfServer()
    {
    }

    MdfServer::~MdfServer()
    {
    }

    void MdfServer::createScene01()
    {
        mScene = std::make_unique<Common::Scenes::Scene01>(mWorldManager);
        // mWorldManager.initialize( mMaxBodiesLimit );
        mScene->createScene();
        // mShip = mScene->createShip();
    }

}  // namespace MaxDoF::Server
