// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_Scenes_Scenes01_H_
#define MaxDoF_Scenes_Scenes01_H_

#include "Scenes/Scene.h"

namespace MaxDoF::Scenes
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
        World::Entity createShip() override;
        void createSphere();
    };
}  // namespace MaxDoF::Scenes

#endif
