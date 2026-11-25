// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <cstdint>
#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Math/Vec3.h>

namespace MaxDoF::Common::Types::Level
{
    struct Graph
    {
        using Vertex = JPH::RVec3;
        using VertexKey = std::size_t;

        struct Triangle
        {
            using Key = std::size_t;
            using VertexKeys = std::array<VertexKey, 3>;
            using Vertices = std::array<Vertex, 3>;

            VertexKeys vertexKeys;
        };

        struct OrientedTriangle
        {
            using Key = std::size_t;

            enum class Type : int
            {
                SolidRock,          // За границей интерфейса находится глухая, монолитная порода астероида (камень)
                OpenSpace,          // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
                UnDefined           // Неопределено
            };

            Triangle::Key triangleId;
            Type type{Type::UnDefined};

            // false — прямой обход вершин (как заложено в Triangle)
            // true — инвертированный обход вершин (зеркальный разворот)
            bool inverted{false};
        };

        struct Node
        {
            using Key = std::size_t;
            using OrientedTriangleKeys = std::vector<OrientedTriangle::Key>;

            enum class Type
            {
                cAntiprism,
                cTetrahedron
            };

            Type type;
            OrientedTriangleKeys orientedTriangleKeys;
            bool inverted{false};
        };

        using Nodes = std::vector<Node>;
        using OrientedTriangles = std::vector<OrientedTriangle>;
        using Triangles = std::vector<Triangle>;
        using Vertices = std::vector<Vertex>;

        Nodes nodes; // Сквозной массив узлов
        OrientedTriangles orientedTriangles; // Сквозной массив ориентированных треугольникев
        Triangles triangles; // Сквозной массив треугольникев
        Vertices vertices; // Сквозной массив точек
    };
}
