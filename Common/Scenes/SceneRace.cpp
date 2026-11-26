// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Scenes/SceneRace.h"

#include "Common/Maps/RaceGenerator.h"
#include "Common/Meshes/MeshBuilder.h"

namespace MaxDoF::Common::Scenes
{
    SceneRace::SceneRace(World::Manager& worldManager) :
        Scene(worldManager)
    {
    }

    SceneRace::~SceneRace()
    {
    }
#ifdef MAXDOF_CLIENT_BUILD
    void SceneRace::createStaticLights()
    {
        auto* sceneManager = mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager();
        float ambientPower = 0.5f; // Ваш множитель яркости

        sceneManager->setAmbientLight(
            Ogre::ColourValue(0.3f, 0.35f, 0.4f) * ambientPower, // Цвет верхней полусферы (небо)
            Ogre::ColourValue(0.2f, 0.18f, 0.15f) * ambientPower, // Цвет нижней полусферы (отражение от земли)
            Ogre::Vector3(0.0f, 1.0f, 0.0f),       // Направление "вверх"
            1.0f                                   // Множитель яркости (EnvMapScale)
        );
    }
#endif
    void SceneRace::CreateScene()
    {

#ifdef MAXDOF_CLIENT_BUILD
        // createStaticLights();
#endif

        mWorldManager.getPhysicsSystem()->SetContactListener( &mBattleLogic.GetMyContactListener() );

        if(mWorldManager.PhysicsHavePlace())
            mShip = new Ships::Ship(mWorldManager);
        else
            return;

        createBoxes({0.0, 0.0, 0.0});

        Types::Level::Graph graph;
        Types::MeshData meshData;
        Maps::RaceGenerator::GenerateGraph(graph);
        Meshes::MeshBuilder::CreateMeshData(meshData, graph);

        if(mWorldManager.PhysicsHavePlace())
            mWorldManager.createMesh(meshData);
        else
            return;
    }
}
