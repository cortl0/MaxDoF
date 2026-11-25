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
            {
                .type = Types::Level::Graph::Node::Type::cAntiprism,
                .orientedTriangleKeys =
                {
                    config.triangleKeys[0],
                    config.triangleKeys[1]
                },
                .inverted = config.invertPipe
            }
        );
    }
}
