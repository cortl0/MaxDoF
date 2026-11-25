// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>

namespace MaxDoF::Common::Types
{
    struct MeshData
    {
        using VertexKey = std::size_t;

        struct Vertex
        {
            double x, y, z;
#ifdef MAXDOF_CLIENT_BUILD
            float u, v;
#endif
        };

        using Vertices = std::vector<Vertex>;
        using VertexKeys = std::vector<VertexKey>;
        Vertices vertices;
        VertexKeys vertexKeys;
    };
}
