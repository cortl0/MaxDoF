// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "GameCommon/LevelMeshes/MeshData.h"
namespace MaxDoF::GameCommon::LevelMeshes
{
    class MeshBuilder
    {
        struct UVVertex {
            JPH::Vec3 pos;
            float u, v;
        };

        static void AddPhysicalTriangle(
            MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, 
            const UVVertex& p0, 
            const UVVertex& p1, 
            const UVVertex& p2
        );

        static std::array<JPH::Vec3, 3> GenerateTriangleVertices(
            const JPH::Vec3& position, 
            const JPH::Vec3& U, 
            const JPH::Vec3& V, 
            float angleOffset, 
            float radius
        );

    public:
        static void CreateMeshData(GameCommon::LevelMeshes::MeshData&);
        static void BuildPipe(GameCommon::LevelMeshes::MeshData&, const JPH::RVec3& begin, const JPH::RVec3& end, float radius);
        static void ConnectTriangles(
            MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, 
            const std::array<JPH::Vec3, 3>& verticesBegin, 
            const std::array<JPH::Vec3, 3>& verticesEnd
        );
    };
}
