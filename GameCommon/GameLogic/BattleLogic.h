// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "GameCommon/World/ContactListener.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameCommon::GameLogic
{
    class BattleLogic
    {
        GameCommon::World::Manager& mWorldManager;
        GameCommon::World::MyContactListener mMyContactListener;

    public:
        BattleLogic(GameCommon::World::Manager&);
        ~BattleLogic();
        GameCommon::World::MyContactListener& GetMyContactListener(){ return mMyContactListener; }
        void Update();
    };
}
