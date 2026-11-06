// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "DeathmatchGraphGenerator.h"
#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Pipe.h"
#include "Tetrahedron.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void DeathmatchGraphGenerator::GenerateGraph(GameCommon::Types::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // 1. РАДИУС НАШЕЙ ЕДИНСТВЕННОЙ КОМНАТЫ
        const float cArenaRadius = 50.0f;

        Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                Types::LevelGraphs::InterfaceType::SolidRock,
                Types::LevelGraphs::InterfaceType::SolidRock,
                Types::LevelGraphs::InterfaceType::SolidRock,
                Types::LevelGraphs::InterfaceType::UnDefined
            });
        auto& tetrahedronGraph = graph.nodes.back();
        tetrahedronGraph.orientedInterfaces.back().interfaceType = Types::LevelGraphs::InterfaceType::UnDefined;
        graph.interfaces[tetrahedronGraph.orientedInterfaces.back().interfaceId].nodeB =
            static_cast<Types::LevelGraphs::NodeKey>(Types::LevelGraphs::InterfaceType::UnDefined);

        Pipe pipe;
        pipe.SetVetricesAndInterfaces(graph, graph.interfaces[tetrahedronGraph.orientedInterfaces.back().interfaceId],
            JPH::RVec3 {0.0f, 0.0f, -cArenaRadius / 2.0f}, cArenaRadius);
        pipe.FillGraph(graph, {false, true}, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::SolidRock});

        auto& pipeGraph = graph.nodes.back();
        pipeGraph.inverted = true;
        pipeGraph.orientedInterfaces.back().inverted = false;

        pipeGraph.orientedInterfaces.front().interfaceType = Types::LevelGraphs::InterfaceType::UnDefined;
    }
}
