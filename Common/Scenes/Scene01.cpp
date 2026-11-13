// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Scenes/Scene01.h"

#include "Common/Maps/Samples/DeathmatchGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchAsteroidGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchAsteroidRotGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchSpaceGraphGenerator.h"
#include "Common/Maps/Samples/PipeGraphGenerator.h"
#include "Common/Maps/Samples/TetrahedronGraphGenerator.h"
#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Tetrahedron.h"

namespace MaxDoF::Common::Scenes
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
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(mapHalfSize, 0.0f, 0.0f), // Смещение вправо по X
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, -mapHalfSize, 0.0f), // Смещение вниз по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, mapHalfSize, 0.0f), // Смещение вверх по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, -mapHalfSize), // Смещение назад по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false);
        }

        {
            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, mapHalfSize), // Смещение вперед по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(Common::Types3d::EntityType::cAsteroid, pose, JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false);
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
        // createMapBoundaries();
#ifdef MAXDOF_CLIENT_BUILD
        // createStaticLights();
#endif
        int size = 2;
        double offset = 30.0;

        for(int i = -size; i <= size; ++i)
            for(int j = -size; j <= size; ++j)
                for(int k = -size; k <= size; ++k)
                {
                    Types::PhysPose pose
                    {
                        .position = JPH::RVec3( i * 1.1f, j * 1.1f, -offset - k * 1.1f ),
                        .orientation = JPH::Quat::sIdentity()
                    };
                    mWorldManager.createCube(Common::Types3d::EntityType::cOtherObject, pose, Types::cUnitScaleVector * 0.5f, 1000.0f);
                }

        Types::PhysPose pose
        {
            .position = JPH::RVec3( 0.0f, 0.0f, -12.0f ),
            .orientation = JPH::Quat::sIdentity()
        };
        mWorldManager.createSphere(Common::Types3d::EntityType::cOtherObject, pose, 0.5f, 1000.0f);
        auto& shipEntity = createShipEntity();
        mShip = new Common::Ships::Ship(mWorldManager,
#ifdef MAXDOF_CLIENT_BUILD
            mWorldManager.getGraphics(),
#endif
            shipEntity);

        Common::LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.GenerateVetrices(JPH::RVec3::sZero(), 1.0);
        auto places = tetrahedron.GetVertices();
        const double pipePos = 15.0;
        const double pipePosZ = 75.0;
        Common::Types::LevelGraphs::PureLevelGraph levelGraph;
        Common::Maps::PipeGraphGenerator::GenerateGraph(levelGraph, {pipePos, pipePos, -pipePosZ}, {false, true}, true, JPH::Quat{0.0f, 0.0f, 0.25881905f, 0.96592583f});
        Common::Maps::PipeGraphGenerator::GenerateGraph(levelGraph, {-pipePos, pipePos, -pipePosZ}, {false, true}, true, JPH::Quat{0.0f, 0.0f, -0.25881905f, 0.96592583f});
        Common::Maps::PipeGraphGenerator::GenerateGraph(levelGraph, {pipePos, -pipePos, -pipePosZ}, {true, false}, false, JPH::Quat{0.0f, 0.0f, 0.25881905f, 0.96592583f});
        Common::Maps::PipeGraphGenerator::GenerateGraph(levelGraph, {-pipePos, -pipePos, -pipePosZ}, {true, false}, false, JPH::Quat{0.0f, 0.0f, -0.25881905f, 0.96592583f});
        Common::Maps::TetrahedronGraphGenerator::GenerateGraph(levelGraph, places[0] * 100.0);
        Common::Maps::DeathmatchGraphGenerator::GenerateGraph(levelGraph, places[1] * 140.0);
        Common::Maps::DeathmatchSpaceGraphGenerator::GenerateGraph(levelGraph, places[2] * 100.0);
        Common::Maps::DeathmatchAsteroidGraphGenerator::GenerateGraph(levelGraph, places[3] * 100.0);
        Common::Maps::DeathmatchAsteroidRotGraphGenerator::GenerateGraph(levelGraph, {0.0, 0.0, 0.0});

        Common::Types3d::MeshData meshData;
        MaxDoF::Common::Meshes::MeshBuilder::CreateMeshData(meshData, levelGraph);
        mWorldManager.createMesh(meshData);
    }

    Types::Entity& Scene01::createShipEntity()
    {
        return mWorldManager.createSphere(Common::Types3d::EntityType::cShip,
            Common::Types::PhysPose
            {
                .position = JPH::RVec3{0.0, 0.0, 0.0},
                .orientation = JPH::Quat{0.0f, 0.0f, 0.0f, 1.0f}.Normalized()
            }
            , 0.5f, 1000.0f, true);
    }
}
