// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Pipe.h"

#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::LevelGraphs
{
    void Pipe::FillGraph(Types::Level::Graph& graph, const Config& config)
    {
        graph.nodes.push_back(
            Types::Level::Graph::Node
            {
                .id = graph.nodes.size(),
                .type = Types::Level::Graph::Node::Type::PipeTunnel,
                .orientedInterfacesKeys = Types::Level::Graph::Node::OrientedTriangleKeys
                {
                    graph.orientedTriangles.size(),
                    graph.orientedTriangles.size() + 1
                },
                .inverted = config.invertPipe
            }
        );

        for(std::size_t i = 0; i < 4; ++i)
        {
            graph.orientedTriangles.push_back
            (
                Types::Level::Graph::OrientedTriangle
                {
                    .id = mTriangleInterfaceKeys[i],
                    .type = config.interfaceTypes[i],
                    .inverted = config.invertInterfaces[i]
                }
            );
        }
    }
}
