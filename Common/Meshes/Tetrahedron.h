// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraph.h"
#include "Primitive.h"

namespace MaxDoF::Common::LevelGraphs
{
    class Tetrahedron : public Primitive
    {
    public:
        using Vertices = std::array<JPH::RVec3, 4>;
        using TrianglesVertexKeys = std::array<Types::Level::Graph::Triangle::VertexKeys, 4>;

        struct Config
        {
            struct Side
            {
                Types::Level::Graph::OrientedTriangle::Type type{};
                bool flip{};
            };

            std::array<Side, 4> sides;
        };

    private:
        Vertices mVertices;
        static constexpr TrianglesVertexKeys mTrianglesVertexKeys
        {{
            {0, 2, 1}, // Грань 0 Левая
            {0, 1, 3}, // Грань 1 Правая
            {0, 3, 2}, // Грань 2 Нижняя
            {1, 2, 3}  // Грань 3 Переднияя
        }};

    public:
        Tetrahedron();

        static void AddVertices(Types::Level::Graph&, const Vertices&);
        const Vertices& GetVertices() const noexcept { return mVertices; }
        void GenerateVetrices(JPH::RVec3 offset, double tetrahedronSphereRadius);
        void FillGraph(Types::Level::Graph &, const Config&) const;
    };
}
