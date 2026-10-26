// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include "GameCommon/Scenes/Scene.h"

namespace MaxDoF::GameCommon::Scenes
{
    class Scene01 : public Scene
    {
        static constexpr double mapHalfSize{75.0f};

        void createMapBoundaries();

    public:
        Scene01(World::Manager&);
        ~Scene01() override;
#ifdef MAXDOF_CLIENT_BUILD
        void createStaticLights();
#endif
        void createScene() override;
        Types::Entity& createShipEntity() override;
    };
}
