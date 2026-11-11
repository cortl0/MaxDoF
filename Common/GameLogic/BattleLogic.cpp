// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/GameLogic/BattleLogic.h"

namespace MaxDoF::Common::GameLogic
{
    BattleLogic::BattleLogic(Common::World::Manager& worldManager) : mWorldManager(worldManager)
    {
    }

    BattleLogic::~BattleLogic()
    {
    }

    bool BulletWith(const Types3d::EntityType type)
    {
        return !(type == Types3d::EntityType::cAsteroid || type == Types3d::EntityType::cShip);
    }

    std::pair<bool, bool> NeedDestroy(const Types3d::EntityType type1, const Types3d::EntityType type2)
    {
        if(type1 == Types3d::EntityType::cBullet)
        {
            return {true, BulletWith(type2)};
        }

        if(type2 == Types3d::EntityType::cBullet)
        {
            return {BulletWith(type1), true};
        }

        return {false, false};
    }

    void BattleLogic::Update()
    {
        for(auto& contact : mMyContactListener.FlushContacts())
        {
            auto index1 = contact.body1.GetIndex();
            auto index2 = contact.body2.GetIndex();

            std::pair<bool, bool> needDestroy = NeedDestroy(
                static_cast<Types3d::EntityType>(contact.userData1),
                static_cast<Types3d::EntityType>(contact.userData2)
            );

            if(needDestroy.first)
                mWorldManager.destroyEntity(index1);

            if(needDestroy.second)
                mWorldManager.destroyEntity(index2);
        }
    }
}
