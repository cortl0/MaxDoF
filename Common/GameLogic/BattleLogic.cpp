// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "GameCommon/GameLogic/BattleLogic.h"

namespace MaxDoF::GameCommon::GameLogic
{
    BattleLogic::BattleLogic(GameCommon::World::Manager& worldManager) : mWorldManager(worldManager)
    {
    }

    BattleLogic::~BattleLogic()
    {
    }

    void BattleLogic::Update()
    {
        for(auto& contact : mMyContactListener.FlushContacts())
        {
            auto index1 = contact.body1.GetIndex();
            auto index2 = contact.body2.GetIndex();
            mWorldManager.destroyEntity(index1);
            mWorldManager.destroyEntity(index2);
        }
    }
}
