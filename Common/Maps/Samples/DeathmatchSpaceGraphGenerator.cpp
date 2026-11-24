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
    void DeathmatchSpaceGraphGenerator::GenerateGraph(Common::Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        const auto& interfacesBaseIndex = graph.triangles.size();
        Common::LevelGraphs::Tetrahedron tetrahedron;
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

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 5);
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
            pipe.SetTriangleInterfaceKeys({
                graph.triangles[interfacesBaseIndex + i].id,
                graph.triangles[interfacesBaseIndex + i + 4].id
            });
            pipe.FillGraph(graph,
                {
                    {false, true}, true,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined,
                        Types::Level::Graph::OrientedTriangle::Type::UnDefined
                    }
                }
            );
        }
    }
}
