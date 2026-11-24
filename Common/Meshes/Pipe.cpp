// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Pipe.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::LevelGraphs
{
    void Pipe::FillGraph(Types::LevelGraphs::PureLevelGraph& graph, const Config& config)
    {
        graph.nodes.push_back(
            Types::LevelGraphs::VolumeNode
            {
                .id = graph.nodes.size(),
                .type = Types::LevelGraphs::NodeType::PipeTunnel,
                .orientedInterfaces = Types::LevelGraphs::VolumeNode::OrientedInterfaces
                {
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = mTriangleInterfaceKeys.front(),
                        .interfaceType = config.interfaceTypes.front(),
                        .inverted = config.invertInterfaces.front()
                    },
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = mTriangleInterfaceKeys.back(),
                        .interfaceType = config.interfaceTypes.back(),
                        .inverted = config.invertInterfaces.back()
                    }
                },
                .inverted = config.invertPipe
            }
        );
    }
}
