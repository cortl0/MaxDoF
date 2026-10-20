// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameCommon::Scenes
{
    class Scene
    {
    protected:
        World::Manager& mWorldManager;

    public:
        Scene(World::Manager&);
        virtual ~Scene();
        virtual void createScene() = 0;
        virtual Types::Entity& createShipEntity() = 0;
    };
}
