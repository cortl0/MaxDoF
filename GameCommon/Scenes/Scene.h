// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include "GameClient/Ships/Ship.h"
#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameCommon::Scenes
{
    class Scene
    {
    protected:
        World::Manager& mWorldManager;
        GameClient::Ships::Ship* mShip;

    public:
        Scene(World::Manager&);
        virtual ~Scene();
        virtual void createScene() = 0;
        virtual Types::Entity& createShipEntity() = 0;
        GameClient::Ships::Ship& getShip() { return *mShip; }
    };
}
