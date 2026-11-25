// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/DeathmatchGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void DeathmatchGraphGenerator::GenerateGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 50.0f;
        const auto& interfacesBaseIndex = graph.orientedTriangles.size();
        LevelGraphs::Tetrahedron tetrahedron;
        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph
        (
            graph,
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
            triangle.SetVertices(graph, graph.triangles[graph.orientedTriangles[interfacesBaseIndex + i].triangleId]);
            triangle.Translate(-triangle.CalculateNormal() * cArenaRadius);
            triangle.FillGraphVerticesAndCreateInterface(graph, {Types::Level::Graph::OrientedTriangle::Type::OpenSpace, false});
            pipe.SetTriangleKeys({
                    graph.orientedTriangles[interfacesBaseIndex + i].triangleId,
                    graph.orientedTriangles[interfacesBaseIndex + i + 4].triangleId
            });
            pipe.FillGraph(graph,
                {
                    {true, false}, false,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::SolidRock
                    }
                }
            );
        }
    }
}
