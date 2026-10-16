// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Server_H_
#define MaxDoF_Server_H_

#include "Scenes/Scene01.h"
#include "World/Entity.h"
#include "World/Manager.h"

// Maximum Degrees of Freedom
namespace MaxDoF::Server
{
    class Server
    {
        World::Manager mWorldManager;
        std::unique_ptr<Scenes::Scene> mScene;

    public:
        Server();
        ~Server();
        void createScene01();
    };
}  // namespace MaxDoF::Server

#endif
