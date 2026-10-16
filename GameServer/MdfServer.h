// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Server_H_
#define MaxDoF_Server_H_

#include "GameCommon/Scenes/Scene01.h"
#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

// Maximum Degrees of Freedom
namespace MaxDoF::GameServer
{
    class MdfServer
    {
        World::Manager mWorldManager;
        std::unique_ptr<Scenes::Scene> mScene;

    public:
        MdfServer();
        ~MdfServer();
        void createScene01();
    };
}  // namespace MaxDoF::GameServer

#endif
