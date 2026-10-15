// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "Scenes/Scene.h"

namespace MaxDoF::Scenes
{
    Scene::Scene(World::Manager& worldManager) :
        mWorldManager(worldManager)
    {
    }

    Scene::~Scene()
    {
    }
}  // namespace MaxDoF::Scenes
