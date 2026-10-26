// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/Scenes/Scene.h"

namespace MaxDoF::GameCommon::Scenes
{
    Scene::Scene(World::Manager& worldManager) :
        mWorldManager(worldManager)
    {
    }

    Scene::~Scene()
    {
    }
}
