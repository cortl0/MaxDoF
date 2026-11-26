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
    void DeathmatchSpaceGraphGenerator::GenerateGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        const auto& nodesBaseIndex = graph.nodes.size();
        LevelGraphs::Tetrahedron tetrahedron;
        LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                .sides
                {{
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false}
                }}
            }
        );

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 5);
        tetrahedron.FillGraph(graph,
            {
                .sides
                {{
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false}
                }}
            }
        );

        for (size_t i = 0; i < 4; ++i)
        {
            const auto& tetrahedronNode0 = graph.nodes[nodesBaseIndex];
            const auto& tetrahedronNode1 = graph.nodes[nodesBaseIndex + 1];
            pipe.FillGraph(graph,
                {
                    true,
                    .begin
                    {
                        .key = tetrahedronNode0.orientedTriangleKeys[i],
                        .type = Types::Level::Graph::OrientedTriangle::Type::cUnDefined,
                        .flip = false
                    },
                    .end
                    {
                        .key = tetrahedronNode1.orientedTriangleKeys[i],
                        .type = Types::Level::Graph::OrientedTriangle::Type::cSolidRock,
                        .flip = true
                    }
                }
            );
        }
    }
}
