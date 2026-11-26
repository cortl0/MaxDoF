// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Scenes/Scene.h"

namespace MaxDoF::Common::Scenes
{
    Scene::Scene(World::Manager& worldManager) :
        mWorldManager(worldManager),
        mBattleLogic(worldManager)
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::Update()
    {
        mBattleLogic.Update();
    }

    void Scene::createBoxes(JPH::RVec3 offset)
    {
        int size = 2;

        for(int i = -size; i <= size; ++i)
            for(int j = -size; j <= size; ++j)
                for(int k = -size; k <= size; ++k)
                {
                    if(std::abs(i) < 2 && std::abs(j) < 2 && std::abs(k) < 2)
                        continue;

                    if(!mWorldManager.PhysicsHavePlace())
                        return;

                    mWorldManager.createCube(Types::Entity::Type::cOtherObject,
                        {
                            .position = JPH::RVec3(offset.GetX() + i * 1.1f, offset.GetY() + j * 1.1f, offset.GetZ() + k * 1.1f),
                            .orientation = JPH::Quat::sIdentity()
                        },
                        Types::cUnitScaleVector * 0.5f, 1000.0f
                    );
                }
    }
}
