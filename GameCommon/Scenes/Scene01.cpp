// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/Scenes/Scene01.h"

namespace MaxDoF::GameCommon::Scenes
{
    Scene01::Scene01(World::Manager& worldManager) : 
        Scene(worldManager)
    {
    }

    Scene01::~Scene01()
    {
    }

    void Scene01::createCube(Types::PhysPose pose)
    {
        mWorldManager.createCube(pose, Types::cUnitScaleVector * 0.5f, 1000.0f);
    }

#ifdef MAXDOF_CLIENT_BUILD
    void Scene01::createLight()
    {
        //mWorldManager.createLight();
    }
#endif

    void Scene01::createScene()
    {
#ifdef MAXDOF_CLIENT_BUILD
        createLight();
#endif
        for(int i = -5; i < 5; ++i)
            for(int j = -5; j < 5; ++j)
                for(int k = -5; k < 5; ++k)
                    {
                        Types::PhysPose pose
                        {
                            .position = JPH::RVec3( i * 1.1f, j * 1.1f, -20.0f - k * 1.1f ),
                            .orientation = JPH::Quat::sIdentity()
                        };
                        createCube(pose);
                    }
        createSphere();
    }

    Types::Entity& Scene01::createShipEntity()
    {
        return mWorldManager.createSphere(Types::cDefaultPhysPose, 0.5f, 1000.0f);
    }

    void Scene01::createSphere()
    {
        Types::PhysPose pose
        {
            .position = JPH::RVec3( 0.0f, 0.0f, -5.0f ),
            .orientation = JPH::Quat::sIdentity()
        };
        mWorldManager.createSphere(pose, 0.5f, 1000.0f);
    }
}
