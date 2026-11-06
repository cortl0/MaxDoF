// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "GameCommon/Types/LevelGraphStructures.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    class Tetrahedron
    {
    public:
        using Vertices = std::array<JPH::RVec3, 4>;
        using TrianglesVerticesIndices = std::array<Types3d::TriangleVerticesIndices, 4>;

    private:
        Vertices vertices;
        TrianglesVerticesIndices interfaces;

    public:
        Tetrahedron();
        const TrianglesVerticesIndices& GetInterfaces(){ return interfaces; }
        const Vertices& GetVertices(){ return vertices; }
        void SetInterfaces(const TrianglesVerticesIndices& interfaces){ this->interfaces = interfaces; }
        void SetVertices(const Vertices& vertices){ this->vertices = vertices; }
        void SetVetricesAndInterfaces(double tetrahedronSphereRadius);
        void FillGraph(Types::LevelGraphs::PureLevelGraph&);
    };
}
