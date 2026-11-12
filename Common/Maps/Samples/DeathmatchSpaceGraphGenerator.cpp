// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/DeathmatchSpaceGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"

namespace MaxDoF::Common::Maps
{
    void DeathmatchSpaceGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;

        Common::LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(offset, cArenaRadius);

        tetrahedron.FillGraph(graph,
            {
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined,
                Types::LevelGraphs::InterfaceType::UnDefined
            });

        const auto& tetrahedronIndex = graph.nodes.size() - 1;

        Common::LevelGraphs::Tetrahedron tetrahedronSecond;
        tetrahedronSecond.SetVetricesAndInterfaces(offset, cArenaRadius * 5);

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
            Common::LevelGraphs::Pipe pipe;
            pipe.SetVetricesAndInterfaces(
                graph,
                graph.interfaces[graph.nodes[tetrahedronIndex].orientedInterfaces[i].interfaceId],
                graph.interfaces[graph.nodes[tetrahedronIndexSecond].orientedInterfaces[i].interfaceId]
            );

            pipe.FillGraph(graph, {false, true}, true, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});

            // auto& pipeGraph = graph.nodes.back();
            // pipeGraph.inverted = true;
            // pipeGraph.orientedInterfaces.back().inverted = false;
            // pipeGraph.orientedInterfaces.front().interfaceType = Types::LevelGraphs::InterfaceType::UnDefined;
        }
    }
}
