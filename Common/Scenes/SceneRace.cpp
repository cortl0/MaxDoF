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

    void SceneRace::CreateScene()
    {
        mWorldManager.getPhysicsSystem()->SetContactListener( &mBattleLogic.GetMyContactListener() );

        if(!mWorldManager.PhysicsHavePlace())
            return;

        mShip = new Common::Ships::Ship(mWorldManager);

        Common::Types::Level::Graph graph;
        Common::Maps::RaceGenerator::GenerateGraph(graph,
            {0.0f, 0.0f, 0.0f}, JPH::Quat{0.0f, 0.0f, 0.25881905f, 0.96592583f},
            {
                {false, true}, true,
                {
                    Common::Types::Level::Graph::Graph::OrientedTriangle::Type::OpenSpace,
                    Common::Types::Level::Graph::Graph::OrientedTriangle::Type::OpenSpace
                }
            }
        );

        Common::Types3d::MeshData meshData;
        MaxDoF::Common::Meshes::MeshBuilder::CreateMeshData(meshData, graph);

        if(!mWorldManager.PhysicsHavePlace())
            return;

        mWorldManager.createMesh(meshData);
    }
}
