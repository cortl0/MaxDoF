// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "GameCommon/LevelMeshes/LevelGraphStructures.h"
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

    public:
        static void ConnectTriangles(
            MaxDoF::GameCommon::LevelMeshes::MeshData& meshData,
            const std::array<JPH::Vec3, 3>& verticesBegin,
            const std::array<JPH::Vec3, 3>& verticesEnd
        );
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
            GameCommon::LevelMeshes::MeshData& meshData,
            const GameCommon::LevelGraphs::PureLevelGraph& graph
        );
        static void AddOrientedWallTriangle(MaxDoF::GameCommon::LevelMeshes::MeshData &meshData, const UVVertex &p0, const UVVertex &p1, const UVVertex &p2, const JPH::Vec3 &localPipeCenter);
        static std::array<JPH::Vec3, 3> GenerateTriangleVertices(
            const JPH::Vec3 &position,
            const JPH::Vec3 &U,
            const JPH::Vec3 &V,
            float angleOffset,
            float radius);
    };
}
