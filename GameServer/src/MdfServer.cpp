// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "server.h"

namespace MaxDoF::Server
{
    Server::Server()
    {
    }

    Server::~Server()
    {
    }

    void Server::createScene01()
    {
        mScene = std::make_unique<Scenes::Scene01>(mWorldManager);
        // mWorldManager.initialize( mMaxBodiesLimit );
        mScene->createScene();
        // mShip = mScene->createShip();
    }

}  // namespace MaxDoF::Server
