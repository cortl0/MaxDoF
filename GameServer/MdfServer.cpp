// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameServer/MdfServer.h"

namespace MaxDoF::GameServer
{
    MdfServer::MdfServer()
    {
    }

    MdfServer::~MdfServer()
    {
    }

    void MdfServer::createScene01()
    {
        mScene = std::make_unique<GameCommon::Scenes::Scene01>(mWorldManager);
        // mWorldManager.initialize( mMaxBodiesLimit );
        mScene->createScene();
        // mShip = mScene->createShip();
    }

}  // namespace MaxDoF::GameServer
