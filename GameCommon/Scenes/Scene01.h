// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameCommon_Scenes_Scene01_H_
#define MaxDoF_GameCommon_Scenes_Scene01_H_

#include "GameCommon/Scenes/Scene.h"

namespace MaxDoF::GameCommon::Scenes
{
    class Scene01 : public Scene
    {
    public:
        Scene01(World::Manager&);
        ~Scene01() override;
        void createCube();
#ifdef MAXDOF_CLIENT_BUILD
        void createLight();
#endif
        void createScene() override;
        Types::Entity createShip() override;
        void createSphere();
    };
}  // namespace MaxDoF::GameCommon::Scenes

#endif
