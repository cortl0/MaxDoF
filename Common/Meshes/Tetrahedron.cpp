// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Tetrahedron.h"

#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::LevelGraphs
{
    Tetrahedron::Tetrahedron()
    {
    }

    void Tetrahedron::AddVertices(Types::Level::Graph& graph, const Vertices& vertices)
    {
        for(const auto& vertex : vertices)
            graph.vertices.push_back(vertex);
    }

    void Tetrahedron::GenerateVetrices(JPH::RVec3 offset, double tetrahedronSphereRadius)
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

        mVertices = {
            JPH::RVec3(0.0f, 0.0f, zBack) + offset,                     // Вершина 0 (Сзади, на оси +Z)
            JPH::RVec3(0.0f, yTop, zFront) + offset,                    // Вершина 1 (Вперед-вверх)
            JPH::RVec3(-edgeLength / 2.0f, yBottom, zFront) + offset,   // Вершина 2 (Вперед-влево-вниз)
            JPH::RVec3(edgeLength / 2.0f, yBottom, zFront) + offset     // Вершина 3 (Вперед-вправо-вниз)
        };
    }

    void Tetrahedron::FillGraph(Types::Level::Graph& graph, const Config& config) const
    {
        const auto baseVertexIndex = graph.vertices.size();
        const auto baseOrientedTriangleIndex = graph.orientedTriangles.size();

        AddVertices(graph, mVertices);

        for(const auto& triangleVertexKeys : mTrianglesVertexKeys)
            graph.AddTriangle
            ({
                triangleVertexKeys[0] + baseVertexIndex,
                triangleVertexKeys[1] + baseVertexIndex,
                triangleVertexKeys[2] + baseVertexIndex
            });

        for(std::size_t i = 0; i < 4; ++i)
            graph.AddOrientedTriangle
            ({
                .triangleKey = baseOrientedTriangleIndex + i,
                .type = config.sides[i].type,
                .flip = config.sides[i].flip
            });

        graph.AddNode
        ({
            .type = Types::Level::Graph::Node::Type::cTetrahedron,
            .orientedTriangleKeys = Types::Level::Graph::Node::OrientedTriangleKeys
            {
                baseOrientedTriangleIndex,
                baseOrientedTriangleIndex + 1,
                baseOrientedTriangleIndex + 2,
                baseOrientedTriangleIndex + 3
            }
        });
    }
}
