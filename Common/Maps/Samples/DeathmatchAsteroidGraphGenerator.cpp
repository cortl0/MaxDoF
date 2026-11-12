// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/DeathmatchAsteroidGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"

namespace MaxDoF::Common::Maps
{
    void DeathmatchAsteroidGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset)
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
        tetrahedronSecond.SetVetricesAndInterfaces(offset, cArenaRadius * 3);

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
                cArenaRadius,
                cArenaRadius
            );

            pipe.FillGraph(graph, {true, false}, false, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});
        }

        const auto& firstPipeIndex = graph.nodes.size() - 4;

        for (size_t i = 0; i < 4; ++i)
        {
            Common::LevelGraphs::Pipe pipe;
            pipe.SetVetricesAndInterfaces(
                graph,
                graph.interfaces[graph.nodes[firstPipeIndex + i].orientedInterfaces.back().interfaceId],
                graph.interfaces[graph.nodes[tetrahedronIndexSecond].orientedInterfaces[i].interfaceId]
            );

            pipe.FillGraph(graph, {true, false}, false, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});
        }
    }
}
