// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/Types3d.h"
#include "Common/Types/LevelGraphStructures.h"

namespace MaxDoF::Common::LevelGraphs
{
    class Pipe
    {
    public:
        using Vertices = std::array<Types3d::TriangleVertices, 2>;
        using TrianglesVerticesIndices = std::array<Types3d::TriangleVerticesIndices, 2>;

    private:
        Vertices vertices;
        TrianglesVerticesIndices interfaces;

    public:
        Pipe();
        const TrianglesVerticesIndices& GetInterfaces(){ return interfaces; }
        const Vertices& GetVertices(){ return vertices; }
        void SetInterfaces(const TrianglesVerticesIndices& interfaces){ this->interfaces = interfaces; }
        void SetVertices(const Vertices& vertices){ this->vertices = vertices; }
        void SetVetricesAndInterfaces(JPH::RVec3 posBegin, JPH::RVec3 posEnd, double radius);
        void SetVetricesAndInterfaces(Types::LevelGraphs::PureLevelGraph&, Types::LevelGraphs::TriangleInterface fitst, double length, double radius);
        void FillGraph(Types::LevelGraphs::PureLevelGraph&, std::array<bool, 2> invertInterfaces, std::array<Types::LevelGraphs::InterfaceType, 2> interfaceTypes);
    };
}
