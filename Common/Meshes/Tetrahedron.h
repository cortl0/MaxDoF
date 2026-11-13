// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraphStructures.h"
#include "Primitive.h"

namespace MaxDoF::Common::LevelGraphs
{
    class Tetrahedron : public Primitive
    {
    public:
        using Vertices = std::array<JPH::RVec3, 4>;
        using TrianglesVerticesIndices = std::array<Types3d::TriangleVerticesIndices, 4>;

    private:
        Vertices vertices;
        static constexpr TrianglesVerticesIndices interfaces
        {{
            {0, 2, 1}, // Грань 0 Левая
            {0, 1, 3}, // Грань 1 Правая
            {0, 3, 2}, // Грань 2 Нижняя
            {1, 2, 3}  // Грань 3 Переднияя
        }};

    public:
        Tetrahedron();
        // const TrianglesVerticesIndices& GetInterfaces() const noexcept { return interfaces; }
        const Vertices& GetVertices() const noexcept { return vertices; }
        // void SetInterfaces(const TrianglesVerticesIndices& interfaces){ this->interfaces = interfaces; }
        // void SetVertices(const Vertices& vertices){ this->vertices = vertices; }
        void GenerateVetrices(JPH::RVec3 offset, double tetrahedronSphereRadius);
        void FillGraph(Types::LevelGraphs::PureLevelGraph&, std::array<Types::LevelGraphs::InterfaceType, 4> interfaceTypes);
    };
}
