// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "DeathmatchSpaceGraphGenerator.h"

#include "Common/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Pipe.h"
#include "Tetrahedron.h"

namespace MaxDoF::Common::LevelGraphs
{
    void DeathmatchSpaceGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph)
    {
        const float cArenaRadius = 10.0f;

        Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(cArenaRadius);

        tetrahedron.FillGraph(graph,
            {
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined
            });

        const auto& tetrahedronIndex = graph.nodes.size() - 1;

        Tetrahedron tetrahedronSecond;
        tetrahedronSecond.SetVetricesAndInterfaces(cArenaRadius * 5);

        tetrahedronSecond.FillGraph(graph,
            {
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined
            });

        const auto& tetrahedronIndexSecond = graph.nodes.size() - 1;

        for (size_t i = 0; i < 4; ++i)
        {
            Pipe pipe;
            pipe.SetVetricesAndInterfaces(
                graph,
                graph.interfaces[graph.nodes[tetrahedronIndex].orientedInterfaces[i].interfaceId],
                graph.interfaces[graph.nodes[tetrahedronIndexSecond].orientedInterfaces[i].interfaceId]
            );

            pipe.FillGraph(graph, {false, true}, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});

            auto& pipeGraph = graph.nodes.back();
            pipeGraph.inverted = true;
            pipeGraph.orientedInterfaces.back().inverted = false;
            pipeGraph.orientedInterfaces.front().interfaceType = Types::LevelGraphs::InterfaceType::UnDefined;
        }
    }
}
