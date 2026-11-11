// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "DeathmatchGraphGenerator.h"
#include "Common/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Pipe.h"
#include "Tetrahedron.h"

namespace MaxDoF::Common::LevelGraphs
{
    void DeathmatchGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph)
    {
        const float cArenaRadius = 50.0f;

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

        for (size_t i = 0; i < 4; ++i)
        {
            graph.interfaces[graph.nodes[tetrahedronIndex].orientedInterfaces[i].interfaceId].nodeB =
                static_cast<Types::LevelGraphs::NodeKey>(Types::LevelGraphs::InterfaceType::UnDefined);

            Pipe pipe;
            pipe.SetVetricesAndInterfaces(
                graph, 
                graph.interfaces[graph.nodes[tetrahedronIndex].orientedInterfaces[i].interfaceId],
                cArenaRadius, 
                cArenaRadius
            );

            pipe.FillGraph(graph, {false, true}, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::SolidRock});

            auto& pipeGraph = graph.nodes.back();
            pipeGraph.inverted = true;
            pipeGraph.orientedInterfaces.back().inverted = false;
            pipeGraph.orientedInterfaces.front().interfaceType = Types::LevelGraphs::InterfaceType::UnDefined;
        }
    }
}
