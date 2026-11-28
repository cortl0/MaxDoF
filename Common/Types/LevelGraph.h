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
        using KeyType = std::size_t;
        using Vertex = JPH::RVec3;
        using VertexKey = KeyType;

        struct Triangle
        {
            using Key = KeyType;
            using VertexKeys = std::array<VertexKey, 3>;
            using Vertices = std::array<Vertex, 3>;

            VertexKeys vertexKeys;
        };

        struct OrientedTriangle
        {
            using Key = KeyType;

            enum class Type : int
            {
                cUnDefined,               // Неопределено
                cOpenSpace = cUnDefined,  // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
                cSolidRock                // За границей интерфейса находится глухая, монолитная порода астероида (камень)
            };

            Triangle::Key triangleKey;
            Type type{};

            // false — прямой обход вершин (как заложено в Triangle)
            // true — инвертированный обход вершин (зеркальный разворот)
            bool flip{};
        };

        struct Node
        {
            using Key = KeyType;
            using OrientedTriangleKeys = std::vector<OrientedTriangle::Key>;

            enum class Type
            {
                cUnDefined,
                cAntiprism,
                cTetrahedron
            };

            Type type{};
            OrientedTriangleKeys orientedTriangleKeys;
            bool inverted{};
            bool invertedAntiprismEnd{};
        };

        using Nodes = std::vector<Node>;
        using OrientedTriangles = std::vector<OrientedTriangle>;
        using Triangles = std::vector<Triangle>;
        using Vertices = std::vector<Vertex>;

    // private:
        Nodes nodes; // Сквозной массив узлов
        OrientedTriangles orientedTriangles; // Сквозной массив ориентированных треугольникев
        Triangles triangles; // Сквозной массив треугольникев
        Vertices vertices; // Сквозной массив точек

    public:
        inline void AddTriangleVertices(const Triangle::Vertices& TriangleVertices)
        {
            for(const auto vertex : TriangleVertices)
                vertices.push_back(vertex);
        }

        inline void AddTriangle(const Triangle::VertexKeys& triangleVertexKeys)
        {
            triangles.push_back({.vertexKeys{triangleVertexKeys}});
        }

        inline void AddOrientedTriangle(const OrientedTriangle& orientedTriangle)
        {
            orientedTriangles.push_back(orientedTriangle);
        }

        inline void AddNode(const Node& node)
        {
            nodes.push_back(node);
        }

        inline const Vertex& GetVertex(const VertexKey vertexKey) const
        {
            return vertices[vertexKey];
        }

        inline const Triangle::VertexKeys& GetTriangleVertexKeys(const Triangle::Key triangleKey) const
        {
            return triangles[triangleKey].vertexKeys;
        }

        inline const Triangle::Key& GetTriangleKey(const OrientedTriangle::Key orientedTriangleKey) const
        {
            return orientedTriangles[orientedTriangleKey].triangleKey;
        }

        inline const Node::OrientedTriangleKeys& GetNodeOrientedTriangleKeys(const Node::Key nodeKey) const
        {
            return nodes[nodeKey].orientedTriangleKeys;
        }

        inline const Node& GetNode(const Node::Key nodeKey) const
        {
            return nodes[nodeKey];
        }

        inline Triangle::Vertices GetTriangleVertices(const Triangle::Key triangleKey) const
        {
            return
            {
                vertices[triangles[triangleKey].vertexKeys[0]],
                vertices[triangles[triangleKey].vertexKeys[1]],
                vertices[triangles[triangleKey].vertexKeys[2]]
            };
        }
    };
}
