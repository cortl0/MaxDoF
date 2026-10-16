// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Scenes_Scene_H_
#define MaxDoF_Scenes_Scene_H_

#include "Types/Entity.h"
#include "World/Manager.h"

namespace MaxDoF::Scenes
{
    class Scene
    {
    protected:
        World::Manager& mWorldManager;

    public:
        Scene(World::Manager&);
        virtual ~Scene();
        virtual void createScene() = 0;
        virtual Types::Entity createShip() = 0;
    };
}  // namespace MaxDoF::Scenes

#endif
