// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/DeathmatchAsteroidGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void DeathmatchAsteroidGraphGenerator::GenerateGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        const auto& interfacesBaseIndex = graph.orientedTriangles.size();
        const auto& nodesBaseIndex = graph.nodes.size();
        LevelGraphs::Tetrahedron tetrahedron;
        LevelGraphs::Triangle triangle;
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

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 3);
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
            const auto& tetrahedronNode = graph.nodes[nodesBaseIndex];
            triangle.SetVertices(graph, graph.triangles[graph.orientedTriangles[interfacesBaseIndex + i].triangleKey]);
            triangle.Translate(-triangle.CalculateNormal() * cArenaRadius);
            triangle.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, false});
            pipe.FillGraph(graph,
                {
                    .begin
                    {
                        .key = tetrahedronNode.orientedTriangleKeys[i]
                    },
                    .end
                    {
                        .key = graph.orientedTriangles.size() - 1
                    },
                    false
                }
            );
        }

        for (size_t i = 0; i < 4; ++i)
        {
            const auto& pipeNode = graph.nodes[nodesBaseIndex + 2 + i];
            const auto& tetrahedronNode = graph.nodes[nodesBaseIndex + 1];
            pipe.FillGraph(graph,
                {
                    .begin
                    {
                        .key = pipeNode.orientedTriangleKeys[1]
                    },
                    .end
                    {
                        .key = tetrahedronNode.orientedTriangleKeys[i]
                    },
                    false
                }
            );
        }
    }
}
