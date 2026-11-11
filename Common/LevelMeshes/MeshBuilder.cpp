// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/LevelMeshes/MeshBuilder.h"

#include <array>
#include <random>
#include <vector>

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::LevelMeshes
{
    void MeshBuilder::AddNonPlanarQuad(Common::Types3d::MeshData& meshData, bool reverseDiagonal,
        const UVVertex& a, const UVVertex& b, const UVVertex& c, const UVVertex& d)
    {
        if(reverseDiagonal)
        {
            AddTriangle(meshData, a, b, d);
            AddTriangle(meshData, b, c, d);
        }
        else
        {
            AddTriangle(meshData, a, b, c);
            AddTriangle(meshData, a, c, d);
        }
    }

    void MeshBuilder::AddAntiprism(
        Common::Types3d::MeshData& meshData, bool reverseDiagonals,
        const std::array<UVVertex, 3>& a, const std::array<UVVertex, 3>& b)
    {
        AddNonPlanarQuad(meshData, reverseDiagonals, a[1], a[0], b[0], b[1]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[2], a[1], b[1], b[2]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[0], a[2], b[2], b[0]);
    }

    void MeshBuilder::AddTriangle(Common::Types3d::MeshData& meshData, const UVVertex& p0, const UVVertex& p1, const UVVertex& p2)
    {
        std::array<UVVertex, 3> pts = { p0, p1, p2 };
        for (int i = 0; i < 3; ++i)
        {
            MaxDoF::Common::Types3d::MeshData::Vertex v;
            v.x = pts[i].pos.GetX();
            v.y = pts[i].pos.GetY();
            v.z = pts[i].pos.GetZ();
#ifdef MAXDOF_CLIENT_BUILD
            v.u  = pts[i].u;
            v.v  = pts[i].v;
#endif
            meshData.vertices.push_back(v);
            meshData.indices.push_back(meshData.vertices.size() - 1);
        }
    }

    void MeshBuilder::AddAntiprism(MaxDoF::Common::Types3d::MeshData& meshData,
    const Types::LevelGraphs::PureLevelGraph& graph,
    const Types::LevelGraphs::VolumeNode& pipeNode,
    const Types::LevelGraphs::TriangleInterface& begin,
    const Types::LevelGraphs::TriangleInterface& end)
    {
        const auto& interface = graph.interfaces[begin.id];
        const auto& interfaceEnd = graph.interfaces[end.id];

        // 1. Заполняем позиции вершин для обоих торцов
        std::array<UVVertex, 3> a, b;

        for (int i = 0; i < 3; ++i)
        {
            a[i].pos = graph.vertices[interface.verticesIndices[i]];
            b[i].pos = graph.vertices[interfaceEnd.verticesIndices[i]];
        }

        if (pipeNode.inverted)
        {
            auto tempA = a[1].pos;
            a[1].pos = a[2].pos;
            a[2].pos = tempA;

            auto tempB = b[1].pos;
            b[1].pos = b[2].pos;
            b[2].pos = tempB;
        }

        // 2. Вычисляем реальные размеры в метрах (1.0f в координатах = 1 метр)
        // Расстояние по окружности для каждой вершины (учитывает даже разносторонние треугольники)
        float u0 = 0.0f;
        float u1 = (a[1].pos - a[0].pos).Length();
        float u2 = u1 + (a[2].pos - a[1].pos).Length();

        // Длина сегмента антипризмы между центрами торцов
        auto centerA = (a[0].pos + a[1].pos + a[2].pos) / 3.0f;
        auto centerB = (b[0].pos + b[1].pos + b[2].pos) / 3.0f;
        float segmentLength = (centerB - centerA).Length();

        // 3. Назначаем UV-координаты в метрах (1 метр = 1.0 UV)
        a[0].u = u0; a[0].v = 0.0f;
        a[1].u = u1; a[1].v = 0.0f;
        a[2].u = u2; a[2].v = 0.0f;

        b[0].u = u0; b[0].v = segmentLength;
        b[1].u = u1; b[1].v = segmentLength;
        b[2].u = u2; b[2].v = segmentLength;

        AddAntiprism(meshData, false, a, b);
    }

    // 3. Генерация вершин торца (без изменений)
    Types3d::TriangleVertices MeshBuilder::GenerateTriangleVertices(
        const JPH::RVec3& position, const JPH::RVec3& U, const JPH::RVec3& V, float angleOffset, float radius)
    {
        const float PI = 3.1415926535f;
        Types3d::TriangleVertices vertices;
        for (int i = 0; i < 3; ++i)
        {
            float angle = (120.0f * i * PI / 180.0f) + angleOffset;
            float x2d = radius * std::cos(angle);
            float y2d = radius * std::sin(angle);
            vertices[i] = position + x2d * U + y2d * V;
        }
        return vertices;
    }

    void MeshBuilder::CreateMeshData(
        Common::Types3d::MeshData& meshData,
        const Common::Types::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace Common::Types::LevelGraphs;

        for(const auto& node : graph.nodes)
        {
            // =========================================================================
            // ЭТАП 1: СБОРКА СЛОЖНОЙ ОБЪЕМНОЙ ГЕОМЕТРИИ (СТЕНЫ ТРУБ)
            // =========================================================================
            if(node.type == NodeType::PipeTunnel)
            {
                if(node.orientedInterfaces.size() < 2)
                    MaxDoF_LOG_AND_ABORT("bad data")

                // Натягиваем 6 боковых прямоугольников-складок
                AddAntiprism(meshData, graph, node,
                    graph.interfaces[node.orientedInterfaces[0].interfaceId],
                    graph.interfaces[node.orientedInterfaces[1].interfaceId]
                );
            }
            // Для тетраэдра на этом этапе ничего делать не нужно — у него нет боковых складок,
            // его геометрия полностью состоит из плоских граней-интерфейсов!

            // =========================================================================
            // ЭТАП 2: УНИВЕРСАЛЬНАЯ СБОРКА ЗАГЛУШЕК ДЛЯ АБСОЛЮТНО ВСЕХ УЗЛОВ ГРАФА
            // =========================================================================
            // Проходим по всем узлам еще раз и заставляем каждый из них честно
            // проверить свои интерфейсы на наличие глухих тупиков
            for (const auto& orientedInterface : node.orientedInterfaces)
            {
                const auto& face = graph.interfaces[orientedInterface.interfaceId];
                // Если грань официально ведет в монолитный камень — строим заглушку!
                if (orientedInterface.interfaceType == static_cast<Types::LevelGraphs::InterfaceType>(Types::LevelGraphs::InterfaceType::SolidRock))
                {
                    const auto& pos0 = graph.vertices[face.verticesIndices[0]];
                    const auto& pos1 = graph.vertices[face.verticesIndices[1]];
                    const auto& pos2 = graph.vertices[face.verticesIndices[2]];

                    // 1. Строим ортонормированный базис (оси U и V в плоскости треугольника)
                    auto edge1 = pos1 - pos0;
                    auto edge2 = pos2 - pos0;

                    float len1 = edge1.Length();
                    // Защита от вырожденного треугольника
                    auto axisU = (len1 > 0.0001f) ? (edge1 / len1) : edge1;

                    // Нормаль плоскости и перпендикулярная ось V внутри этой же плоскости
                    auto normal = edge1.Cross(edge2).Normalized();
                    auto axisV = normal.Cross(axisU);

                    // 2. Проецируем вершины на локальные оси в метрах (1.0f = 1 метр)
                    // Вершина 0 — начало координат (0, 0)
                    // Вершина 1 лежит строго на оси U на расстоянии len1
                    // Вершина 2 получает точные координаты (u, v) без искажения углов
                    float u2 = edge2.Dot(axisU);
                    float v2 = edge2.Dot(axisV);

                    MeshBuilder::UVVertex p0{ pos0, 0.0f, 0.0f };
                    MeshBuilder::UVVertex p1{ pos1, len1, 0.0f };
                    MeshBuilder::UVVertex p2{ pos2, u2,   v2   };

                    // Единое пуленепробиваемое правило для труб, тетраэдров и комнат:
                    if (orientedInterface.inverted)
                    {
                        AddTriangle(meshData, p0, p2, p1);
                    }
                    else
                    {
                        AddTriangle(meshData, p0, p1, p2);
                    }
                }
            }
        }
    }
}
