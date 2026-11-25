// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/TetrahedronGraphGenerator.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Tetrahedron.h"

namespace MaxDoF::Common::Maps
{
    void TetrahedronGraphGenerator::GenerateGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 50.0f;
        LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                {
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock
                },
                { false, false, false, false }
            }
        );
    }

    void TetrahedronGraphGenerator::GenerateInvertedGraph(Types::Level::Graph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                {
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                    Types::Level::Graph::OrientedTriangle::Type::SolidRock
                },
                { true, true, true, true }
            }
        );
    }
}
