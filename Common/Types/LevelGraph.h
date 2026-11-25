// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>
#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include "Common/Types/Types3d.h"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>

namespace MaxDoF::Common::Types::Level
{
    struct Graph
    {
        using Vertex = JPH::RVec3;
        using VertexKey = std::size_t;
        using TriangleVertices = std::array<Vertex, 3>;
        using TriangleVertexKeys = std::array<VertexKey, 3>;
        using UserDataType = JPH::uint64;

        enum class EntityType : UserDataType
        {
            cUndefined,
            cAsteroid,
            cShip,
            cBullet,
            cOtherObject
        };

        struct Triangle
        {
            using Key = std::size_t;

            TriangleVertexKeys verticesIndices;
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

            // false — прямой обход вершин (как заложено в faceLayout)
            // true — инвертированный обход вершин (зеркальный разворот)
            bool inverted{false};
        };

        struct Node
        {
            using Key = std::size_t;
            using OrientedTriangleKeys = std::vector<OrientedTriangle::Key>;

            enum class Type
            {
                PipeTunnel,  // Объёмный узел: Труба-антипризма (6 треугольников складок)
                Tetrahedron  // Объёмный узел: Тетраэдр (4 треугольные грани комнаты)
            };

            Type type;

            // Список ID плоских треугольников-стыков (TriangleInterface), которыми этот объём ограничен.
            // Для PipeTunnel — строго до 2-х интерфейсов (вход и выход).
            // Для Tetrahedron — строго 4 интерфейса (как открытых, так и глухих).
            // Единый вектор ориентированных стыков-интерфейсов для любых фигур
            OrientedTriangleKeys orientedInterfacesKeys;
            bool inverted{false};
        };

        using Nodes = std::vector<Node>;
        using OrientedTriangles = std::vector<OrientedTriangle>;
        using Triangles = std::vector<Triangle>;
        using Vertices = std::vector<Vertex>;

        Nodes nodes; // Все материальные комнаты, коридоры и тупики
        OrientedTriangles orientedTriangles;
        Triangles triangles; // Все плоские швы, стены и порталы между ними
        Vertices vertices; // Сквозной массив точек графа. Все сущности должны ссылаться на него и не иметь своихточек
    };
}
