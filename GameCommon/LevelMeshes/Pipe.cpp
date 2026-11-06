// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Pipe.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    Pipe::Pipe()
    {
    }

    void Pipe::SetVetricesAndInterfaces(JPH::RVec3 posBegin, JPH::RVec3 posEnd, double radius)
    {
        const float cFixedRotation = 3.1415926535f / 6.0f;

        JPH::RVec3 U(1.0f, 0.0f, 0.0f);
        JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        vertices = {
            GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posBegin, U, V, 0.0f, radius),
            GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posEnd, U, V, cFixedRotation, radius)
        };

        interfaces = {{
            {0, 1, 2}, // Торец 0 Передний
            {3, 4, 5}  // Торец 1 Задний
        }};
    }

    void Pipe::SetVetricesAndInterfaces(Types::LevelGraphs::PureLevelGraph& graph, Types::LevelGraphs::TriangleInterface first, double length, double radius)
    {
        JPH::RVec3 U(1.0f, 0.0f, 0.0f);
        JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        const auto& p0 = graph.vertices[first.verticesIndices[0]];
        const auto& p1 = graph.vertices[first.verticesIndices[1]];
        const auto& p2 = graph.vertices[first.verticesIndices[2]];

        auto offset = -(p1 - p0).Cross(p2 - p1).Normalized() * length;

        vertices = {
            Types3d::TriangleVertices
            {
                graph.vertices[first.verticesIndices[0]],
                graph.vertices[first.verticesIndices[1]],
                graph.vertices[first.verticesIndices[2]]
            },
            Types3d::TriangleVertices
            {
                graph.vertices[first.verticesIndices[0]] + offset,
                graph.vertices[first.verticesIndices[1]] + offset,
                graph.vertices[first.verticesIndices[2]] + offset
            }
        };

        interfaces = {{
            {0, 1, 2}, // Торец 0 Передний
            {3, 4, 5}  // Торец 1 Задний
        }};
    }

    void Pipe::FillGraph(Types::LevelGraphs::PureLevelGraph& graph, std::array<bool, 2> invertInterfaces, std::array<Types::LevelGraphs::InterfaceType, 2> interfaceTypes)
    {
        const auto baseVertexIndex = graph.vertices.size();
        const auto baseInterfaceIndex = graph.interfaces.size();

        for(const auto& innerVertices : vertices)
            for(const auto& vertice : innerVertices)
                graph.vertices.push_back(vertice);

        for(const auto& interface : interfaces)
            graph.interfaces.push_back(Types::LevelGraphs::TriangleInterface
                {
                    .id = graph.interfaces.size(),
                    .verticesIndices = Types3d::TriangleVerticesIndices{interface[0] + baseVertexIndex, interface[1] + baseVertexIndex, interface[2] + baseVertexIndex},
                    .nodeA = graph.nodes.size(),
                    .nodeB = static_cast<Types::LevelGraphs::NodeKey>(Types::LevelGraphs::InterfaceType::SolidRock)
                }
            );

        graph.nodes.push_back(
            Types::LevelGraphs::VolumeNode
            {
                .id = graph.nodes.size(),
                .type = Types::LevelGraphs::NodeType::PipeTunnel,
                .orientedInterfaces = Types::LevelGraphs::VolumeNode::OrientedInterfaces
                {
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 0,
                        .interfaceType = interfaceTypes.front(),
                        .inverted = invertInterfaces.front()
                    },
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 1,
                        .interfaceType = interfaceTypes.back(),
                        .inverted = invertInterfaces.back()
                    }
                }
            }
        );
    }
}
