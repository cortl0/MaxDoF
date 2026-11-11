// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/Entities.h"

namespace MaxDoF::Common::World
{
    void Entites::AddEntity(JPH::BodyID bodyId
#ifdef MAXDOF_CLIENT_BUILD
        , Ogre::SceneNode* ogreNode
#endif
    )
    {
        mIndex[bodyId.GetIndex()] = mEntities.size();
        mEntities.emplace_back(bodyId
#ifdef MAXDOF_CLIENT_BUILD
            , ogreNode
#endif
        );
    }

    std::optional<Types::Entity> Entites::RemoveEntity(EntitiesIndexKey entitiesIndexKey)
    {
        std::optional<Types::Entity> entity;

        if (mEntities.empty()) [[unlikely]]
            return {};

        auto it = mIndex.find(entitiesIndexKey);

        if (it == mIndex.end())
            return {};

        auto targetIndex = it->second;
        auto lastIndex = mEntities.size() - 1;
        entity = mEntities[targetIndex];

        if (targetIndex < lastIndex)
        {
            mEntities[targetIndex] = std::move(mEntities[lastIndex]);
            auto movedEntityBodyIndex = mEntities[targetIndex].physId.GetIndex();
            mIndex[movedEntityBodyIndex] = targetIndex;
        }

        mEntities.pop_back();
        mIndex.erase(it);

        return entity;
    }

    void Entites::Reserve(std::size_t maxBodies)
    {
        mEntities.reserve(maxBodies);
        mIndex.reserve(maxBodies * 2);
        mIndex.max_load_factor(static_cast<float>(maxBodies));
    }
}
