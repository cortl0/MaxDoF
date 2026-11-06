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

        // Стабильный базис плоскости сечения трубы (X-вправо, Y-вверх)
        JPH::RVec3 U(1.0f, 0.0f, 0.0f);
        JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        vertices = {{
           GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posBegin, U, V, 0.0f, radius),
           GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posEnd, U, V, cFixedRotation, radius)
        }};

        interfaces = {{
            {0, 1, 2}, // Вход
            {3, 4, 5}  // Выход
        }};
    }

    void Pipe::SetVetricesAndInterfaces(Types::LevelGraphs::PureLevelGraph& graph, Types::LevelGraphs::TriangleInterface fitst, JPH::RVec3 length, double radius)
    {
        const float cFixedRotation = 3.1415926535f / 6.0f;

        // Стабильный базис плоскости сечения трубы (X-вправо, Y-вверх)
        JPH::RVec3 U(1.0f, 0.0f, 0.0f);
        JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        // vertices = {{
        //     graph.vertices fitst.vertices,
        //     fitst.vertices[0] + length, fitst.vertices[1] + length, fitst.vertices[2] + length
        // }};

        // interfaces = {{
        //     {0, 1, 2}, // Вход
        //     {3, 4, 5}  // Выход
        // }};
    }

    void Pipe::FillGraph(Types::LevelGraphs::PureLevelGraph& graph)
    {
        const auto baseVertexIndex = graph.vertices.size();
        const auto baseInterfaceIndex = graph.interfaces.size();

        for(const auto& innerVertices : vertices)
            for(const auto& vertice : innerVertices)
                graph.vertices.push_back(vertice);

        for(const auto& interface : interfaces)
            graph.interfaces.push_back(Types::LevelGraphs::TriangleInterface{
                .id = graph.interfaces.size(),
                .verticesIndices = Types3d::TriangleVerticesIndices{interface[0] + baseVertexIndex, interface[1] + baseVertexIndex, interface[2] + baseVertexIndex},
                .nodeA = graph.nodes.size(),
                .nodeB = Types::LevelGraphs::cNodeSolidRock
            });

        graph.nodes.push_back(
            Types::LevelGraphs::VolumeNode
            {
                .id = graph.nodes.size(),
                .type = Types::LevelGraphs::NodeType::PipeTunnel,
                .interfaceIds = Types::LevelGraphs::VolumeNode::OrientedInterfaces
                {
                    Types::LevelGraphs::OrientedInterface{.interfaceId = baseInterfaceIndex + 0, .inverted = false},
                    Types::LevelGraphs::OrientedInterface{.interfaceId = baseInterfaceIndex + 1, .inverted = false}
                }
            }
        );
    }
}
