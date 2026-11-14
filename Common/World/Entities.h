// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <optional>

#include "Common/Types/Entity.h"

namespace MaxDoF::Common::World
{
    class Entites
    {
    private:
        using EntitiesIndexKey = JPH::uint32;
        using EntitiesKey = std::size_t;
        using EntitiesContainer = std::vector<Types::Entity>;
        using Index = std::unordered_map<EntitiesIndexKey, EntitiesKey>;

        // TODO: [Optimization] Заменить std::unordered_map в контейнере сущностей на Sparse Set.
        // Использовать 3 плоских вектора (mDense, mDenseToSparse, mSparse) вместо хэш-таблицы.
        // Jolt выдаёт последовательные BodyID::GetIndex(), поэтому прямой доступ по индексу
        // уберёт аллокации в куче и ускорит поиск/удаление до O(1) без промахов кэша.
        EntitiesContainer mEntities;
        Index mIndex;

    public:
        void AddEntity(JPH::BodyID bodyId
#ifdef MAXDOF_CLIENT_BUILD
            , Ogre::SceneNode* ogreNode
#endif
        );

        EntitiesContainer& GetEntities(){ return mEntities; }
        std::optional<Types::Entity> RemoveEntity(EntitiesIndexKey entitiesIndexKey);
        void Reserve(std::size_t maxBodies);
    };
}
