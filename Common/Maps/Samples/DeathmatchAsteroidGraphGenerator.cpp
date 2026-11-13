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
    void DeathmatchAsteroidGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset)
    {
        const float cArenaRadius = 10.0f;
        const auto& interfacesBaseIndex = graph.interfaces.size();
        Common::LevelGraphs::Tetrahedron tetrahedron;
        Common::LevelGraphs::Triangle triangle;
        Common::LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph, {
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined
        });

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 3);
        tetrahedron.FillGraph(graph, {
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined
        });

        for (size_t i = 0; i < 4; ++i)
        {
            triangle.SetVertices(graph, graph.interfaces[interfacesBaseIndex + i]);
            triangle.Translate(-triangle.CalculateNormal() * cArenaRadius);
            triangle.FillGraphVerticesAndCreateInterface(graph);
            pipe.SetTriangleInterfaceKeys({
                graph.interfaces[interfacesBaseIndex + i].id,
                graph.interfaces[graph.interfaces.size() - 1].id
            });
            pipe.FillGraph(graph, {true, false}, false, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});
        }

        for (size_t i = 0; i < 4; ++i)
        {
            pipe.SetTriangleInterfaceKeys({
                graph.interfaces[interfacesBaseIndex + 8 + i].id,
                graph.interfaces[interfacesBaseIndex + 4 + i].id
            });
            pipe.FillGraph(graph, {true, false}, false, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});
        }
    }
}
