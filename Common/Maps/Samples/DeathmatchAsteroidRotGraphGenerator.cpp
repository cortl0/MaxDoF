// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/DeathmatchAsteroidRotGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void DeathmatchAsteroidRotGraphGenerator::GenerateGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 5.0f;
        const auto& interfacesBaseIndex = graph.orientedTriangles.size();
        const auto& nodesBaseIndex = graph.nodes.size();
        LevelGraphs::Tetrahedron tetrahedron;
        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                {
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined
                },
                { false, false, false, false }
            }
        );

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 10.0);
        tetrahedron.FillGraph(graph,
            {
                {
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                    Types::Level::Graph::OrientedTriangle::Type::UnDefined
                },
                { false, false, false, false }
            }
        );

        for (size_t i = 0; i < 4; ++i)
        {
            const auto& tetrahedronNode = graph.nodes[nodesBaseIndex];
            triangle.SetVertices(graph, graph.triangles[graph.orientedTriangles[interfacesBaseIndex + i].triangleId]);
            auto normal = triangle.CalculateNormal();
            triangle.RotateAroundPivot(triangle.CalculateMiddle(), triangle.CalculateRotationQuat(-normal, -15.0f));
            triangle.Translate(-normal * cArenaRadius * 1.0);
            triangle.FillGraphVerticesAndCreateInterface(graph, {Types::Level::Graph::OrientedTriangle::Type::OpenSpace, false});
            pipe.FillGraph(graph,
                {
                    {true, false}, false,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined
                    },
                    {
                        tetrahedronNode.orientedTriangleKeys[i],
                        graph.orientedTriangles.size() - 1
                    }
                }
            );
        }

        for (size_t i = 0; i < 4; ++i)
        {
            const auto& pipeNode = graph.nodes[nodesBaseIndex + 2 + i];
            const auto& tetrahedronNode = graph.nodes[nodesBaseIndex + 1];
            pipe.FillGraph(graph,
                {
                    {true, false}, false,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined
                    },
                    {
                        pipeNode.orientedTriangleKeys[1],
                        tetrahedronNode.orientedTriangleKeys[i]
                    }
                }
            );
        }
    }
}
