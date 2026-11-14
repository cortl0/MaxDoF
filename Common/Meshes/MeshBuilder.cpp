// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Meshes/MeshBuilder.h"

#include <array>
#include <random>
#include <vector>

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::Meshes
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
        std::array<JPH::DVec3, 3> posA, rawPosB;

        for (int i = 0; i < 3; ++i)
        {
            posA[i] = graph.vertices[interface.verticesIndices[i]];
            rawPosB[i] = graph.vertices[interfaceEnd.verticesIndices[i]];
        }

    if (pipeNode.inverted)
    {
        std::swap(posA.at(1), posA.at(2));
        std::swap(rawPosB.at(1), rawPosB.at(2));
    }

        // 2. Выравниваем торцы по минимальному расстоянию (убираем скручивание 120°/240°)
        int bestShift = 0;
        double minTotalDist = 1e30;
        for (int shift = 0; shift < 3; ++shift)
        {
            double distSum = 0.0;
            for (int i = 0; i < 3; ++i)
            {
                distSum += (rawPosB[(i + shift) % 3] - posA[i]).Length();
            }
            if (distSum < minTotalDist)
            {
                minTotalDist = distSum;
                bestShift = shift;
            }
        }

        std::array<JPH::DVec3, 3> posB;
        for (int i = 0; i < 3; ++i)
        {
            posB[i] = rawPosB[(i + bestShift) % 3];
        }

        // 3. Строим каждую из 3 боковых сторон со своей локальной 2D-разверткой 1:1 в метрах
        for (int i = 0; i < 3; ++i)
        {
            int next = (i + 1) % 3;

            const auto& a0 = posA[i];
            const auto& a1 = posA[next];
            const auto& b0 = posB[i];
            const auto& b1 = posB[next];

            // Локальная ось U идет строго вдоль нижнего ребра стороны (a0 -> a1)
            JPH::DVec3 edgeU = a1 - a0;
            double lenU = edgeU.Length();
            JPH::DVec3 axisU = (lenU > 0.0001) ? (edgeU / lenU) : edgeU;

            // Вектор подъема вдоль боковой стороны
            JPH::DVec3 edgeUp = b0 - a0;

            // Нормаль боковой грани и перпендикулярная ось V строго в плоскости этой грани
            JPH::DVec3 normal = edgeU.Cross(edgeUp);
            double normLen = normal.Length();
            if (normLen > 0.0001)
                normal = normal / normLen;

            JPH::DVec3 axisV = normal.Cross(axisU);

            // Проецируем все 4 угла боковой стороны на её локальные оси U и V с явным кастом в float
            UVVertex va0{ a0, 0.0f, 0.0f };
            UVVertex va1{ a1, static_cast<float>(lenU), 0.0f };

            JPH::DVec3 diffB0 = b0 - a0;
            UVVertex vb0{
                b0,
                static_cast<float>(diffB0.Dot(axisU)),
                static_cast<float>(diffB0.Dot(axisV))
            };

            JPH::DVec3 diffB1 = b1 - a0;
            UVVertex vb1{
                b1,
                static_cast<float>(diffB1.Dot(axisU)),
                static_cast<float>(diffB1.Dot(axisV))
            };

            // Добавляем 2 треугольника текущей стороны
            AddTriangle(meshData, va0, va1, vb1);
            AddTriangle(meshData, va0, vb1, vb0);
        }
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
