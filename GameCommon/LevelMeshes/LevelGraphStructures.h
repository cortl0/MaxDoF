// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <vector>
#include <cstdint>

#include "GameCommon/Types/Types3d.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    using NodeKey = uint32_t;
    using TriangleInterfaceKey = uint32_t;
    // =========================================================================
    // СИСТЕМНЫЕ КОНСТАНТЫ СВЯЗЕЙ ДЛЯ ГРАНЕЙ
    // =========================================================================

    // За границей интерфейса находится глухая, монолитная порода астероида (камень)
    constexpr NodeKey cNodeSolidRock = -1;

    // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
    constexpr NodeKey cNodeAsteroidSurface = -2;

    // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
    constexpr NodeKey cNodeOpenSpace = -3;

    // =========================================================================
    // ПЕРЕЧИСЛЕНИЕ ТИПОВ ОБЪЕМНЫХ ФИГУР
    // =========================================================================
    enum class NodeType
    {
        PipeTunnel, // Объёмный узел: Труба-антипризма (6 треугольников складок)
        Tetrahedron // Объёмный узел: Тетраэдр (4 треугольные грани комнаты)
    };

    // =========================================================================
    // 1. БЕЗДУШНАЯ ГРАНЬ (РЕБРО ГРАФА)
    // =========================================================================
    struct TriangleInterface
    {
        TriangleInterfaceKey id;
        Types3d::TriangleVertices vertices;

        // Математическое направление ребра: ИЗ внешнего пространства ВНУТРЬ узла nodeA.
        // Нормаль этого треугольника по умолчанию смотрит внутрь nodeA.
        NodeKey nodeA = cNodeSolidRock;
        NodeKey nodeB = cNodeSolidRock;
    };

    struct OrientedInterface
    {
        TriangleInterfaceKey interfaceId;

        // false — прямой обход вершин (как заложено в faceLayout)
        // true — инвертированный обход вершин (зеркальный разворот)
        bool inverted;
    };

    // =========================================================================
    // 2. БОГАТЫЙ УЗЕЛ ГРАФА — Объёмная физическая сущность лабиринта
    // =========================================================================
    struct VolumeNode
    {
        NodeKey id;
        NodeType type;

        // Список ID плоских треугольников-стыков (TriangleInterface), которыми этот объём ограничен.
        // Для PipeTunnel — строго до 2-х интерфейсов (вход и выход).
        // Для Tetrahedron — строго 4 интерфейса (как открытых, так и глухих).
        // Единый вектор ориентированных стыков-интерфейсов для любых фигур
        std::vector<OrientedInterface> interfaceIds;
    };

    // =========================================================================
    // 3. СТРУКТУРА СЕТИ ВСЕГО УРОВНЯ
    // =========================================================================
    struct PureLevelGraph
    {
        std::vector<VolumeNode>        nodes;      // Все материальные комнаты, коридоры и тупики
        std::vector<TriangleInterface> interfaces; // Все плоские швы, стены и порталы между ними
    };
}
