// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/World/ContactListener.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Common::GameLogic
{
    class BattleLogic
    {
        Common::World::Manager& mWorldManager;
        Common::World::MyContactListener mMyContactListener;

    public:
        BattleLogic(Common::World::Manager&);
        ~BattleLogic();
        Common::World::MyContactListener& GetMyContactListener(){ return mMyContactListener; }
        void Update();
    };
}
