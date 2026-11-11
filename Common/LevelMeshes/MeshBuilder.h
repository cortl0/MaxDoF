// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "GameCommon/Types/LevelGraphStructures.h"

namespace MaxDoF::GameCommon::LevelMeshes
{
    class MeshBuilder
    {
        struct UVVertex {
            JPH::RVec3 pos;
            float u, v;
        };

    public:
        static void AddNonPlanarQuad(GameCommon::Types3d::MeshData &meshData, bool reverseDiagonal,
            const UVVertex &a, const UVVertex &b, const UVVertex &c, const UVVertex &d);
        static void AddPipeWall(GameCommon::Types3d::MeshData &meshData, bool reverseDiagonals,
            const std::array<UVVertex, 3> &verticesBegin, const std::array<UVVertex, 3> &verticesEnd);
        static void AddTriangle(GameCommon::Types3d::MeshData& meshData, const UVVertex& p0, const UVVertex& p1, const UVVertex& p2);
        static void ConnectTriangles(GameCommon::Types3d::MeshData& meshData,
            const Types::LevelGraphs::PureLevelGraph& graph,
            const Types::LevelGraphs::VolumeNode& pipeNode,
            const Types::LevelGraphs::TriangleInterface& TriangleInterface,
            const Types3d::TriangleVertices& verticesEnd);
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
            GameCommon::Types3d::MeshData& meshData,
            const GameCommon::Types::LevelGraphs::PureLevelGraph& graph
        );
        // static void AddOrientedWallTriangle(MaxDoF::GameCommon::LevelMeshes::MeshData &meshData, const UVVertex &p0, const UVVertex &p1, const UVVertex &p2, const JPH::Vec3 &localPipeCenter);
        static Types3d::TriangleVertices GenerateTriangleVertices(
            const JPH::RVec3 &position,
            const JPH::RVec3 &U,
            const JPH::RVec3 &V,
            float angleOffset,
            float radius);
    };
}
