// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Tetrahedron.h"

namespace MaxDoF::Common::LevelGraphs
{
    Tetrahedron::Tetrahedron()
    {
    }

    void Tetrahedron::SetVetricesAndInterfaces(double tetrahedronSphereRadius)
    {
        constexpr double sqrt3 = 1.7320508;
        constexpr double sqrt6 = 2.4494897;

        // Длина ребра правильного тетраэдра при заданном радиусе описанной сферы
        const double edgeLength = (4.0 / sqrt6) * tetrahedronSphereRadius;

        // Радиус описанной окружности грани (размер прохода трубы)
        const double gateSize = edgeLength / sqrt3;

        // Расчет пространственного положения плоскостей для Z-ориентированного тетраэдра
        const double zBack = tetrahedronSphereRadius;            // Одиночная вершина сзади (в плюсе по Z)
        const double zFront = -tetrahedronSphereRadius / 3.0;    // Плоскость основания впереди (в минусе по Z)

        // Высоты для правильного треугольника основания на фронтальной плоскости XY
        const double yTop = gateSize;                  // Верхняя точка основания (смотрит вверх по Y)
        const double yBottom = -gateSize / 2.0;       // Нижняя линия основания

        vertices = {
            JPH::RVec3(0.0f, 0.0f, zBack),                     // Вершина 0 (Сзади, на оси +Z)
            JPH::RVec3(0.0f, yTop, zFront),                    // Вершина 1 (Вперед-вверх)
            JPH::RVec3(-edgeLength / 2.0f, yBottom, zFront),   // Вершина 2 (Вперед-влево-вниз)
            JPH::RVec3(edgeLength / 2.0f, yBottom, zFront)     // Вершина 3 (Вперед-вправо-вниз)
        };

        interfaces = {{
            {0, 2, 1}, // Грань 0 Левая
            {0, 1, 3}, // Грань 1 Правая
            {0, 3, 2}, // Грань 2 Нижняя
            {1, 2, 3}  // Грань 3 Переднияя
        }};
    }

    void Tetrahedron::FillGraph(Types::LevelGraphs::PureLevelGraph& graph, std::array<Types::LevelGraphs::InterfaceType, 4> interfaceTypes)
    {
        const auto baseVertexIndex = graph.vertices.size();
        const auto baseInterfaceIndex = graph.interfaces.size();

        for(const auto& vertice : vertices)
            graph.vertices.push_back(vertice);

        for(const auto& interface : interfaces)
            graph.interfaces.push_back(Types::LevelGraphs::TriangleInterface{
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
                .type = Types::LevelGraphs::NodeType::Tetrahedron,
                .orientedInterfaces = Types::LevelGraphs::VolumeNode::OrientedInterfaces
                {
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 0,
                        .interfaceType = interfaceTypes[0],
                        .inverted = false
                    },
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 1,
                        .interfaceType = interfaceTypes[1],
                        .inverted = false
                    },
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 2,
                        .interfaceType = interfaceTypes[2],
                        .inverted = false
                    },
                    Types::LevelGraphs::OrientedInterface
                    {
                        .interfaceId = baseInterfaceIndex + 3,
                        .interfaceType = interfaceTypes[3],
                        .inverted = false
                    }
                }
            }
        );
    }
}
