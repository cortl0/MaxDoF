// https://Github/cortl0/MaxDoF
// GPL-3.0 license

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
