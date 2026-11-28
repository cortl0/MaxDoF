// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Scenes/SceneTest.h"

#include "Common/Maps/Samples/DeathmatchGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchAsteroidGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchAsteroidRotGraphGenerator.h"
#include "Common/Maps/Samples/DeathmatchSpaceGraphGenerator.h"
#include "Common/Maps/Samples/PipeGraphGenerator.h"
#include "Common/Maps/Samples/TetrahedronGraphGenerator.h"
#include "Common/Maps/Samples/TriangleGraphGenerator.h"
#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Tetrahedron.h"
#include "Common/Types/Entity.h"

namespace MaxDoF::Common::Scenes
{
    SceneTest::SceneTest(World::Manager& worldManager) :
        Scene(worldManager)
    {
    }

    SceneTest::~SceneTest()
    {
    }

    void SceneTest::createMapBoundaries()
    {
        static constexpr float mapWallHalfThickness{0.5f};

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(-mapHalfSize, 0.0f, 0.0f),
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false
            );
        }

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(mapHalfSize, 0.0f, 0.0f), // Смещение вправо по X
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapWallHalfThickness, mapHalfSize, mapHalfSize), 10000.0f, false
            );
        }

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, -mapHalfSize, 0.0f), // Смещение вниз по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false
            );
        }

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, mapHalfSize, 0.0f), // Смещение вверх по Y
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapHalfSize, mapWallHalfThickness, mapHalfSize), 10000.0f, false
            );
        }

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, -mapHalfSize), // Смещение назад по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false
            );
        }

        {
            if(!mWorldManager.PhysicsHavePlace())
                return;

            Types::PhysPose pose{
                .position = JPH::RVec3(0.0f, 0.0f, mapHalfSize), // Смещение вперед по Z
                .orientation = JPH::Quat::sIdentity()
            };
            mWorldManager.createCube(
                Types::Entity::Type::cAsteroid, pose,
                JPH::Vec3(mapHalfSize, mapHalfSize, mapWallHalfThickness), 10000.0f, false
            );
        }
    }

#ifdef MAXDOF_CLIENT_BUILD
    void SceneTest::createStaticLights()
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

    void SceneTest::CreateScene()
    {
        mWorldManager.getPhysicsSystem()->SetContactListener( &mBattleLogic.GetMyContactListener() );

        if(!mWorldManager.PhysicsHavePlace())
            return;

        mShip = new Ships::Ship(mWorldManager);

        // createMapBoundaries();
#ifdef MAXDOF_CLIENT_BUILD
        // createStaticLights();
#endif
        createBoxes({0.0, 0.0, -50.0});

        if(!mWorldManager.PhysicsHavePlace())
            return;

        mWorldManager.createSphere(Types::Entity::Type::cOtherObject, Types::PhysPose
            {
                .position = JPH::RVec3( 0.0f, 0.0f, -30.0f ),
                .orientation = JPH::Quat::sIdentity()
            }, 0.5f, 1000.0f);

        LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.GenerateVetrices(JPH::RVec3::sZero(), 1.0);
        auto places = tetrahedron.GetVertices();

        JPH::RVec3 tetrahedronOffset{places[0] * 100.0};
        JPH::RVec3 deathmatchOffset{places[1] * 140.0};
        JPH::RVec3 deathmatchSpaceOffset{places[2] * 100.0};
        JPH::RVec3 deathmatchAsteroidOffset{places[3] * 100.0};

        createBoxes(tetrahedronOffset);
        createBoxes(deathmatchOffset);
        createBoxes(deathmatchSpaceOffset);
        createBoxes(deathmatchAsteroidOffset);

        Types::Level::Graph graph;
        Maps::TriangleGraphGenerator::GenerateGraph(graph, {0.0, 0.0, -120.0});
        Maps::PipeGraphGenerator::GenerateGraph(graph);
        Maps::TetrahedronGraphGenerator::GenerateInvertedGraph(graph, -tetrahedronOffset);
        Maps::TetrahedronGraphGenerator::GenerateGraph(graph, tetrahedronOffset);
        Maps::DeathmatchGraphGenerator::GenerateGraph(graph, deathmatchOffset);
        Maps::DeathmatchSpaceGraphGenerator::GenerateGraph(graph, deathmatchSpaceOffset);
        Maps::DeathmatchAsteroidGraphGenerator::GenerateGraph(graph, deathmatchAsteroidOffset);
        Maps::DeathmatchAsteroidRotGraphGenerator::GenerateGraph(graph, JPH::RVec3::sZero());

        Types::MeshData meshData;
        Meshes::MeshBuilder::CreateMeshData(meshData, graph);

        if(!mWorldManager.PhysicsHavePlace())
            return;

        mWorldManager.createMesh(meshData);
    }
}
