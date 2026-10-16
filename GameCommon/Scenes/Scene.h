// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameCommon_Scenes_Scene_H_
#define MaxDoF_GameCommon_Scenes_Scene_H_

#include "GameCommon/Types/Entity.h"
#include "GameCommon/World/Manager.h"

namespace MaxDoF::GameCommon::Scenes
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
}  // namespace MaxDoF::GameCommon::Scenes

#endif
