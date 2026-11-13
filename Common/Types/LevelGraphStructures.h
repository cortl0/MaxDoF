// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>
#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include "Common/Types/Types3d.h"

namespace MaxDoF::Common::Types::LevelGraphs
{
    using NodeKey = std::size_t;
    using TriangleInterfaceKey = std::size_t;
    using Vertices = std::vector<Types3d::Vertex>;
    // =========================================================================
    // СИСТЕМНЫЕ КОНСТАНТЫ СВЯЗЕЙ ДЛЯ ГРАНЕЙ
    // =========================================================================

    enum class InterfaceType : NodeKey
    {
        SolidRock =       std::numeric_limits<NodeKey>::max() - 1, // За границей интерфейса находится глухая, монолитная порода астероида (камень)
        AsteroidSurface = std::numeric_limits<NodeKey>::max() - 2, // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
        OpenSpace =       std::numeric_limits<NodeKey>::max() - 3, // За границей интерфейса находится открытый космос (сквозной шлюз наружу)
        UnDefined =       std::numeric_limits<NodeKey>::max() - 4  // Неопределено
    };

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
        Types3d::TriangleVerticesIndices verticesIndices;

        void Print() const
        {
            std::cout
                << "====TriangleInterface id: " << std::to_string(id)<< std::endl
                << std::to_string(verticesIndices[0]) << std::endl
                << std::to_string(verticesIndices[1]) << std::endl
                << std::to_string(verticesIndices[2]) << std::endl
                << "====" << std::endl;
        }
    };

    struct OrientedInterface
    {
        TriangleInterfaceKey interfaceId;
        InterfaceType interfaceType{InterfaceType::UnDefined};

        // false — прямой обход вершин (как заложено в faceLayout)
        // true — инвертированный обход вершин (зеркальный разворот)
        bool inverted{false};
    };

    // =========================================================================
    // 2. БОГАТЫЙ УЗЕЛ ГРАФА — Объёмная физическая сущность лабиринта
    // =========================================================================
    struct VolumeNode
    {
        using OrientedInterfaces = std::vector<OrientedInterface>;
        NodeKey id;
        NodeType type;

        // Список ID плоских треугольников-стыков (TriangleInterface), которыми этот объём ограничен.
        // Для PipeTunnel — строго до 2-х интерфейсов (вход и выход).
        // Для Tetrahedron — строго 4 интерфейса (как открытых, так и глухих).
        // Единый вектор ориентированных стыков-интерфейсов для любых фигур
        OrientedInterfaces orientedInterfaces;
        bool inverted{false};
    };

    // =========================================================================
    // 3. СТРУКТУРА СЕТИ ВСЕГО УРОВНЯ
    // =========================================================================
    struct PureLevelGraph
    {
        std::vector<VolumeNode> nodes; // Все материальные комнаты, коридоры и тупики
        std::vector<TriangleInterface> interfaces; // Все плоские швы, стены и порталы между ними
        Vertices vertices; // Сквозной массив точек графа. Все сущности должны ссылаться на него и не иметь своихточек.

        void PrintVertices()
        {
            for(auto i = vertices.size() - vertices.size(); i < vertices.size(); ++i)
            std::cout
                << "====PureLevelGraph::Vertex id: " << std::to_string(i)<< std::endl
                << std::to_string(vertices[i].GetX()) << std::endl
                << std::to_string(vertices[i].GetY()) << std::endl
                << std::to_string(vertices[i].GetZ()) << std::endl
                << "====" << std::endl;
        }
        void PrintInterfaces()
        {
            for(const auto& i : interfaces)
                i.Print();
        }
    };
}
