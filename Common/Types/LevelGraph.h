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

namespace MaxDoF::Common::Types::Level
{
    struct Graph
    {
        struct Triangle
        {
            using Key = std::size_t;

            Key id;
            Types3d::TriangleVerticesIndices verticesIndices;

            void Print() const
            {
                std::cout
                    << "====TriangleInterface id: " << std::to_string(id)<< std::endl
                    << std::to_string(verticesIndices[0]) << std::endl
                    << std::to_string(verticesIndices[1]) << std::endl
                    << std::to_string(verticesIndices[2]) << std::endl
                    << "====" << std::endl;
            }
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

            Triangle::Key id;
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

            Key id;
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
        using Vertices = std::vector<Types3d::Vertex>;

        Nodes nodes; // Все материальные комнаты, коридоры и тупики
        OrientedTriangles orientedTriangles;
        Triangles triangles; // Все плоские швы, стены и порталы между ними
        Vertices vertices; // Сквозной массив точек графа. Все сущности должны ссылаться на него и не иметь своихточек

        void PrintVertices()
        {
            for(auto i = vertices.size() - vertices.size(); i < vertices.size(); ++i)
            std::cout
                << "====PureLevelGraph::Vertex id: " << std::to_string(i)<< std::endl
                << std::to_string(vertices[i].GetX()) << std::endl
                << std::to_string(vertices[i].GetY()) << std::endl
                << std::to_string(vertices[i].GetZ()) << std::endl
                << "====" << std::endl;
        }

        void PrintInterfaces()
        {
            for(const auto& i : triangles)
                i.Print();
        }
    };
}
