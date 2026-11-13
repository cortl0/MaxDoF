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
        const auto& interfacesBaseIndex = graph.interfaces.size();
        Common::LevelGraphs::Tetrahedron tetrahedron;
        Common::LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(offset, cArenaRadius);
        tetrahedron.FillGraph(graph, {
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined
        });

        tetrahedron.GenerateVetrices(offset, cArenaRadius * 5);
        tetrahedron.FillGraph(graph,{
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined,
            Types::LevelGraphs::InterfaceType::UnDefined
        });

        for (size_t i = 0; i < 4; ++i)
        {
            pipe.SetTriangleInterfaceKeys({
                graph.interfaces[interfacesBaseIndex + i].id,
                graph.interfaces[interfacesBaseIndex + i + 4].id
            });
            pipe.FillGraph(graph, {false, true}, true, {Types::LevelGraphs::InterfaceType::UnDefined, Types::LevelGraphs::InterfaceType::UnDefined});
        }
    }
}
