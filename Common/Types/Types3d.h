// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <iostream>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>

namespace MaxDoF::Common::Types3d
{
    using UserDataType = JPH::uint64;

    enum class EntityType : UserDataType
    {
        cUndefined,
        cAsteroid,
        cShip,
        cBullet,
        cOtherObject
    };

    using Vertex = JPH::RVec3;
    using TriangleVertices = std::array<Vertex, 3>;
    using VerticesIndexKey = std::size_t;
    using TriangleVerticesIndices = std::array<VerticesIndexKey, 3>;

    struct MeshData
    {
        struct Vertex
        {
            double x, y, z;
#ifdef MAXDOF_CLIENT_BUILD
            float u, v;
#endif
        };

        using Vertices = std::vector<Vertex>;
        using VerticeIndices = std::vector<VerticesIndexKey>;
        Vertices vertices;
        VerticeIndices indices;
    };
}
