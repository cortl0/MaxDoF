// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "GameClient/Ships/Ship.h"
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
