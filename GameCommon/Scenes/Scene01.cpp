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

    void Scene01::createMapBoundaries()
    {
        static constexpr float mapWallHalfThickness{0.5f};

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(-mapHalfSize, 0.0f, 0.0f),
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(mapHalfSize, 0.0f, 0.0f), // Смещение вправо по X
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, -mapHalfSize, 0.0f), // Смещение вниз по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, mapHalfSize, 0.0f), // Смещение вверх по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, -mapHalfSize), // Смещение назад по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, mapHalfSize), // Смещение вперед по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(pose, JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false);
        }
    }


#ifdef MAXDOF_CLIENT_BUILD
    void Scene01::createStaticLights()
    {
        constexpr double mapLightHalfSize{mapHalfSize * 0.9f};

        // Lights in corner of map cube
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{ mapLightHalfSize,  mapLightHalfSize,  mapLightHalfSize}); // Передний правый верхний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{-mapLightHalfSize,  mapLightHalfSize,  mapLightHalfSize}); // Передний левый верхний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{ mapLightHalfSize,  mapLightHalfSize, -mapLightHalfSize}); // Задний правый верхний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{-mapLightHalfSize,  mapLightHalfSize, -mapLightHalfSize}); // Задний левый верхний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{ mapLightHalfSize, -mapLightHalfSize,  mapLightHalfSize}); // Передний правый нижний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{-mapLightHalfSize, -mapLightHalfSize,  mapLightHalfSize}); // Передний левый нижний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{ mapLightHalfSize, -mapLightHalfSize, -mapLightHalfSize}); // Задний правый нижний
        mWorldManager.getGraphics().createPointLight(Ogre::Vector3{-mapLightHalfSize, -mapLightHalfSize, -mapLightHalfSize}); // Задний левый нижний
    }
#endif

    void Scene01::createScene()
    {
        createMapBoundaries();
#ifdef MAXDOF_CLIENT_BUILD
        createStaticLights();
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
                        mWorldManager.createCube(pose, Types::cUnitScaleVector * 0.5f, 1000.0f);
                    }

        Types::PhysPose pose
        {
            .position = JPH::RVec3( 0.0f, 0.0f, -12.0f ),
            .orientation = JPH::Quat::sIdentity()
        };
        mWorldManager.createSphere(pose, 0.5f, 1000.0f);
    }

    Types::Entity& Scene01::createShipEntity()
    {
        return mWorldManager.createSphere(Types::cDefaultPhysPose, 0.5f, 1000.0f);
    }
}
