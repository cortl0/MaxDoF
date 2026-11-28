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
    void MeshBuilder::AddNonPlanarQuad(Types::MeshData& meshData, bool reverseDiagonal,
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
        Types::MeshData& meshData, bool reverseDiagonals,
        const std::array<UVVertex, 3>& a, const std::array<UVVertex, 3>& b)
    {
        AddNonPlanarQuad(meshData, reverseDiagonals, a[1], a[0], b[0], b[1]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[2], a[1], b[1], b[2]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[0], a[2], b[2], b[0]);
    }

    void MeshBuilder::AddTriangle(Types::MeshData& meshData, const UVVertex& p0, const UVVertex& p1, const UVVertex& p2)
    {
        std::array<UVVertex, 3> pts = { p0, p1, p2 };
        for (int i = 0; i < 3; ++i)
        {
            Types::MeshData::Vertex v;
            v.x = pts[i].pos.GetX();
            v.y = pts[i].pos.GetY();
            v.z = pts[i].pos.GetZ();
#ifdef MAXDOF_CLIENT_BUILD
            v.u  = pts[i].u;
            v.v  = pts[i].v;
#endif
            meshData.vertexKeys.push_back(meshData.vertices.size());
            meshData.vertices.push_back(v);
        }
    }

    void MeshBuilder::AddAntiprism(Types::MeshData& meshData,
        const Types::Level::Graph& graph,
        const Types::Level::Graph::Node& pipeNode,
        const Types::Level::Graph::Triangle::Key& begin,
        const Types::Level::Graph::Triangle::Key& end)
    {
        const auto& interface = graph.triangles[begin];
        const auto& interfaceEnd = graph.triangles[end];

        // 1. Заполняем позиции вершин для обоих торцов
        std::array<JPH::DVec3, 3> posA, rawPosB;

        for (int i = 0; i < 3; ++i)
        {
            posA[i] = graph.vertices[interface.vertexKeys[i]];
            rawPosB[i] = graph.vertices[interfaceEnd.vertexKeys[i]];
        }

        if (pipeNode.inverted)
        {
            std::swap(posA.at(1), posA.at(2));
            std::swap(rawPosB.at(1), rawPosB.at(2));
        }

        if (pipeNode.invertedAntiprismEnd)
        {
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

        // 3. Строим каждую из 3 боковых сторон с изометрической разверткой и привязкой ко входу (a0 -> a1)
        for (int i = 0; i < 3; ++i)
        {
            int next = (i + 1) % 3;

            const auto& a0 = posA[i];
            const auto& a1 = posA[next];
            const auto& b0 = posB[i];
            const auto& b1 = posB[next];

            double diag01Len = (b1 - a0).Length();
            double diag10Len = (b0 - a1).Length();

            auto projectThirdVertex = [](
                const JPH::DVec3& pOrigin,
                const JPH::DVec3& axisU,
                const JPH::DVec3& pTarget,
                double signV) -> std::pair<float, float>
            {
                JPH::DVec3 vec = pTarget - pOrigin;
                double u = vec.Dot(axisU);
                double v = (vec - axisU * u).Length() * signV;
                return { static_cast<float>(u), static_cast<float>(v) };
            };

            UVVertex va0{ a0, 0.0f, 0.0f };
            UVVertex va1{ a1, 0.0f, 0.0f };
            UVVertex vb0{ b0, 0.0f, 0.0f };
            UVVertex vb1{ b1, 0.0f, 0.0f };

            const bool useDiag01 = (diag01Len <= diag10Len);

            if (useDiag01)
            {
                JPH::DVec3 axisU = (diag01Len > 0.0001) ? ((b1 - a0) / diag01Len) : JPH::DVec3::sAxisX();
                vb1.u = static_cast<float>(diag01Len);

                auto [uA1, vA1] = projectThirdVertex(a0, axisU, a1, -1.0);
                auto [uB0, vB0] = projectThirdVertex(a0, axisU, b0, +1.0);

                va1.u = uA1; va1.v = vA1;
                vb0.u = uB0; vb0.v = vB0;
            }
            else
            {
                JPH::DVec3 axisU = (diag10Len > 0.0001) ? ((b0 - a1) / diag10Len) : JPH::DVec3::sAxisX();
                va1.u = 0.0f; va1.v = 0.0f;
                vb0.u = static_cast<float>(diag10Len); vb0.v = 0.0f;

                auto [uA0, vA0] = projectThirdVertex(a1, axisU, a0, -1.0);
                auto [uB1, vB1] = projectThirdVertex(a1, axisU, b1, +1.0);

                va0.u = uA0; va0.v = vA0;
                vb1.u = uB1; vb1.v = vB1;
            }

            // 4. Жесткий 2D-поворот и сдвиг выкройки: выравниваем входное ребро (va0 -> va1) строго вдоль оси U
            const float originU = va0.u;
            const float originV = va0.v;
            const float edgeU   = va1.u - originU;
            const float edgeV   = va1.v - originV;
            const float edgeLen = std::hypot(edgeU, edgeV);

            if (edgeLen > 0.0001f)
            {
                const float cosA = edgeU / edgeLen;
                const float sinA = edgeV / edgeLen;

                auto alignToEntrance = [&](UVVertex& vert)
                {
                    const float du = vert.u - originU;
                    const float dv = vert.v - originV;
                    vert.u = du * cosA + dv * sinA;
                    vert.v = -du * sinA + dv * cosA;
                };

                alignToEntrance(va0);
                alignToEntrance(va1);
                alignToEntrance(vb0);
                alignToEntrance(vb1);
            }

            if (useDiag01)
            {
                AddTriangle(meshData, va0, va1, vb1);
                AddTriangle(meshData, va0, vb1, vb0);
            }
            else
            {
                AddTriangle(meshData, va0, va1, vb0);
                AddTriangle(meshData, va1, vb1, vb0);
            }
        }
    }

    void MeshBuilder::CreateMeshData
    (
        Types::MeshData& meshData,
        const Types::Level::Graph& graph
    )
    {
        using namespace Types::Level;

        for(const auto& node : graph.nodes)
        {
            // =========================================================================
            // ЭТАП 1: СБОРКА СЛОЖНОЙ ОБЪЕМНОЙ ГЕОМЕТРИИ (СТЕНЫ ТРУБ)
            // =========================================================================
            if(node.type == Graph::Node::Type::cAntiprism)
            {
                if(node.orientedTriangleKeys.size() < 2)
                    MaxDoF_LOG_AND_ABORT("bad data")

                // Натягиваем 6 боковых прямоугольников-складок
                AddAntiprism(meshData, graph, node,
                    graph.orientedTriangles[node.orientedTriangleKeys[0]].triangleKey,
                    graph.orientedTriangles[node.orientedTriangleKeys[1]].triangleKey
                );
            }
        }

        // =========================================================================
        // ЭТАП 2: УНИВЕРСАЛЬНАЯ СБОРКА ЗАГЛУШЕК ДЛЯ АБСОЛЮТНО ВСЕХ УЗЛОВ ГРАФА
        // =========================================================================
        for (const auto& orientedInterface : graph.orientedTriangles)// node.orientedInterfacesKeys)
        {
            const auto& face = graph.triangles[orientedInterface.triangleKey];
            // Если грань официально ведет в монолитный камень — строим заглушку!
            if (orientedInterface.type == Types::Level::Graph::OrientedTriangle::Type::cSolidRock)
            {
                const auto& pos0 = graph.vertices[face.vertexKeys[0]];
                const auto& pos1 = graph.vertices[face.vertexKeys[1]];
                const auto& pos2 = graph.vertices[face.vertexKeys[2]];

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

                if (orientedInterface.flip)
                    AddTriangle(meshData, p0, p2, p1);
                else
                    AddTriangle(meshData, p0, p1, p2);
            }
        }
    }
}
