// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <cstdint>
#include <vector>

namespace MaxDoF::GameCommon::LevelMeshes
{
    struct Vertex
    {
        float x, y, z;
#ifdef MAXDOF_CLIENT_BUILD
        float u, v;
#endif
    };

    struct MeshData
    {
        using Vertices = std::vector<Vertex>;
        using Indices = std::vector<uint32_t>;
        Vertices vertices;
        Indices indices;
    };
}
