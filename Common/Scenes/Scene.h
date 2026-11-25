// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/GameLogic/BattleLogic.h"
#include "Common/Ships/Ship.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Common::Scenes
{
    class Scene
    {
    protected:
        World::Manager& mWorldManager;
        Ships::Ship* mShip;
        GameLogic::BattleLogic mBattleLogic;

    public:
        Scene(World::Manager&);
        virtual ~Scene();

        virtual void CreateScene() = 0;
        Ships::Ship& GetShip() { return *mShip; }
        void Update();
    };
}
