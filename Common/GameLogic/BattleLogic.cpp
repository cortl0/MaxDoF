// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/GameLogic/BattleLogic.h"
#include "Common/Types/LevelGraph.h"

namespace MaxDoF::Common::GameLogic
{
    BattleLogic::BattleLogic(World::Manager& worldManager) : mWorldManager(worldManager)
    {
    }

    BattleLogic::~BattleLogic()
    {
    }

    bool BulletWith(const Types::Entity::Type type)
    {
        return !(
            type == Types::Entity::Type::cAsteroid ||
            type == Types::Entity::Type::cShip
        );
    }

#ifdef MAXDOF_CLIENT_BUILD
    void BattleLogic::CreateSound(uint64_t userData, const JPH::RVec3& contactPoint, const JPH::BodyID& bodyId)
    {
        if(static_cast<Types::Entity::Type>(userData) == Types::Entity::Type::cBullet)
        {
            auto position = mWorldManager.getBodyInterface()->GetPosition(bodyId);
            mWorldManager.CreateSound(position, Game::Types::Sounds::cExplosion);
            return;
        }

        if(static_cast<Types::Entity::Type>(userData) == Types::Entity::Type::cShip)
        {
            auto position = (contactPoint + mWorldManager.getBodyInterface()->GetPosition(bodyId)) / 2.0f;
            mWorldManager.CreateSound(position, Game::Types::Sounds::cBarrel);
            return;
        }
    }
#endif

    std::pair<bool, bool> NeedDestroy(
        const Types::Entity::Type type1,
        const Types::Entity::Type type2
    )
    {
        if(type1 == Types::Entity::Type::cBullet)
        {
            return {true, BulletWith(type2)};
        }

        if(type2 == Types::Entity::Type::cBullet)
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
                static_cast<Types::Entity::Type>(contact.userData1),
                static_cast<Types::Entity::Type>(contact.userData2)
            );

#ifdef MAXDOF_CLIENT_BUILD
            CreateSound(contact.userData1, contact.contactPoint, contact.body1);
            CreateSound(contact.userData2, contact.contactPoint, contact.body2);
#endif
            if(needDestroy.first)
                mWorldManager.destroyEntity(index1);

            if(needDestroy.second)
                mWorldManager.destroyEntity(index2);
        }
    }
}
