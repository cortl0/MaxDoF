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
    void DeathmatchAsteroidGraphGenerator::GenerateGraph(Common::Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        const auto& interfacesBaseIndex = graph.triangles.size();
        Common::LevelGraphs::Tetrahedron tetrahedron;
        Common::LevelGraphs::Triangle triangle;
        Common::LevelGraphs::Pipe pipe;

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

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 3);
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
            triangle.SetVertices(graph, graph.triangles[interfacesBaseIndex + i]);
            triangle.Translate(-triangle.CalculateNormal() * cArenaRadius);
            triangle.FillGraphVerticesAndCreateInterface(graph);
            pipe.SetTriangleInterfaceKeys({
                graph.triangles[interfacesBaseIndex + i].id,
                graph.triangles[graph.triangles.size() - 1].id
            });
            pipe.FillGraph(graph,
                {
                    {true, false}, false,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined
                    }
                }
            );
        }

        for (size_t i = 0; i < 4; ++i)
        {
            pipe.SetTriangleInterfaceKeys({
                graph.triangles[interfacesBaseIndex + 8 + i].id,
                graph.triangles[interfacesBaseIndex + 4 + i].id
            });
            pipe.FillGraph(graph,
                {
                    {true, false}, false,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined
                    }
                }
            );
        }
    }
}
