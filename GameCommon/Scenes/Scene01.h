// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include "GameCommon/Scenes/Scene.h"

namespace MaxDoF::GameCommon::Scenes
{
    class Scene01 : public Scene
    {
    public:
        Scene01(World::Manager&);
        ~Scene01() override;
        void createCube(Types::PhysPose pose);
#ifdef MAXDOF_CLIENT_BUILD
        void createLight();
#endif
        void createScene() override;
        Types::Entity& createShipEntity() override;
        void createSphere();
    };
}
