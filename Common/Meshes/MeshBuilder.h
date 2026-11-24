// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "Common/Types/LevelGraph.h"

namespace MaxDoF::Common::Meshes
{
    class MeshBuilder
    {
        struct UVVertex {
            JPH::RVec3 pos;
            float u, v;
        };

    public:
        static void AddNonPlanarQuad(Common::Types3d::MeshData &meshData, bool reverseDiagonal,
            const UVVertex &a, const UVVertex &b, const UVVertex &c, const UVVertex &d);
        static void AddAntiprism(Common::Types3d::MeshData &meshData, bool reverseDiagonals,
            const std::array<UVVertex, 3> &begin, const std::array<UVVertex, 3> &end);
        static void AddTriangle(Common::Types3d::MeshData& meshData, const UVVertex& p0, const UVVertex& p1, const UVVertex& p2);
        static void AddAntiprism(Common::Types3d::MeshData& meshData,
            const Types::Level::Graph&,
            const Types::Level::Graph::Node& pipeNode,
            const Types::Level::Graph::Triangle& begin,
            const Types::Level::Graph::Triangle& end);

        static void CreateMeshData(Common::Types3d::MeshData&, const Common::Types::Level::Graph&);
    };
}
