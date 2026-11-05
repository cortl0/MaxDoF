// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
// #include <Jolt/Math/Quat.h>
#include <Jolt/Math/Real.h>

namespace MaxDoF::GameCommon::Types3d
{
    using VerticesIndexKey = uint32_t;
    using TriangleVerticesIndices = std::array<VerticesIndexKey, 3>;
    using TriangleVertices = std::array<JPH::RVec3, 3>;

    namespace Pipe
    {
        using Vertices = std::array<TriangleVertices, 2>;
        using TrianglesVerticesIndices = std::array<TriangleVerticesIndices, 2>;
    }

    namespace Tetrahedron
    {
        using Vertices = std::array<JPH::RVec3, 4>;
        using TrianglesVerticesIndices = std::array<TriangleVerticesIndices, 4>;
    }

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
