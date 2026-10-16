// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/Scenes/Scene01.h"

namespace MaxDoF::Scenes
{
    Scene01::Scene01(World::Manager& worldManager) : 
        Scene(worldManager)
    {
    }

    Scene01::~Scene01()
    {
    }

    void Scene01::createCube()
    {
        Types::PhysPose pose
        {
            .position = JPH::RVec3( 1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mWorldManager.createCube(pose, Types::cUnitScaleVector * 0.5f, 1000.0f);
    }

#ifdef MAXDOF_CLIENT_BUILD
    void Scene01::createLight()
    {
        mWorldManager.createLight();
    }
#endif

    void Scene01::createScene()
    {
#ifdef MAXDOF_CLIENT_BUILD
        createLight();
#endif
        createCube();
        createSphere();
    }

    Types::Entity Scene01::createShip()
    {
        return mWorldManager.createSphere(Types::cDefaultPhysPose, 0.5f, 5000.0f);
    }

    void Scene01::createSphere()
    {
        Types::PhysPose pose
        {
            .position = JPH::RVec3( -1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mWorldManager.createSphere(pose, 0.5f, 1000.0f);
    }
}  // namespace MaxDoF::Scenes
