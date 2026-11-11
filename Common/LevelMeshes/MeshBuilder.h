// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "Common/Types/LevelGraphStructures.h"

namespace MaxDoF::Common::LevelMeshes
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
            const Types::LevelGraphs::PureLevelGraph& graph,
            const Types::LevelGraphs::VolumeNode& pipeNode,
            const Types::LevelGraphs::TriangleInterface& begin,
            const Types::LevelGraphs::TriangleInterface& end);
        /**
         * @brief Главная функция трансляции графа в полигональный меш уровня.
         *
         * Проходит по всем объемным узлам (Nodes) графа и последовательно генерирует
         * для них физическую и визуальную геометрию с честными флэт-нормалями и UV.
         *
         * @param meshData Контейнер вершин и индексов, куда будет записан итоговый меш уровня.
         * @param graph Сгенерированный процедурный или шаблонный граф структуры уровня.
         */
        static void CreateMeshData(
            Common::Types3d::MeshData& meshData,
            const Common::Types::LevelGraphs::PureLevelGraph& graph
        );
        // static void AddOrientedWallTriangle(MaxDoF::Common::LevelMeshes::MeshData &meshData, const UVVertex &p0, const UVVertex &p1, const UVVertex &p2, const JPH::Vec3 &localPipeCenter);
        static Types3d::TriangleVertices GenerateTriangleVertices(
            const JPH::RVec3 &position,
            const JPH::RVec3 &U,
            const JPH::RVec3 &V,
            float angleOffset,
            float radius);
    };
}
