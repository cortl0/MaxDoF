// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "GameCommon/LevelMeshes/MeshBuilder.h"

#include <array>
#include <cstdint>
#include <random>
#include <vector>

#include "GameCommon/LevelMeshes/Constants.h"
#include "GameCommon/Utils/SystemUtils.h"

namespace MaxDoF::GameCommon::LevelMeshes
{
    void MeshBuilder::AddNonPlanarQuad(MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, bool reverseDiagonal,
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

    void MeshBuilder::AddPipeWall(
        MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, bool reverseDiagonals,
        const std::array<UVVertex, 3>& a, const std::array<UVVertex, 3>& b)
    {
        AddNonPlanarQuad(meshData, reverseDiagonals, a[1], a[0], b[0], b[1]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[2], a[1], b[1], b[2]);
        AddNonPlanarQuad(meshData, reverseDiagonals, a[0], a[2], b[2], b[0]);
    }

    void MeshBuilder::AddTriangle(GameCommon::LevelMeshes::MeshData& meshData, const UVVertex& p0, const UVVertex& p1, const UVVertex& p2)
    {
        std::array<UVVertex, 3> pts = { p0, p1, p2 };
        for (int i = 0; i < 3; ++i)
        {
            MaxDoF::GameCommon::LevelMeshes::Vertex v;
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

    void MeshBuilder::ConnectTriangles(MaxDoF::GameCommon::LevelMeshes::MeshData& meshData,
        const Types3d::TriangleVertices& verticesBegin, const Types3d::TriangleVertices& verticesEnd)
    {
        // 1. Комбинаторный поиск пар по кратчайшему расстоянию
        const std::array<std::array<int, 3>, 6> permutations = {{
            {0, 1, 2}, {0, 2, 1},
            {1, 0, 2}, {1, 2, 0},
            {2, 0, 1}, {2, 1, 0}
        }};

        float minTotalDistance = FLT_MAX;
        std::array<int, 3> bestPermutation = {0, 1, 2};

        for (const auto& p : permutations) {
            float dist0 = (verticesBegin[0] - verticesEnd[p[0]]).Length();
            float dist1 = (verticesBegin[1] - verticesEnd[p[1]]).Length();
            float dist2 = (verticesBegin[2] - verticesEnd[p[2]]).Length();

            float currentSum = dist0 + dist1 + dist2;
            if (currentSum < minTotalDistance) {
                minTotalDistance = currentSum;
                bestPermutation = p;
            }
        }

        // 2. Подготовка базовых UV-вершин
        std::array<UVVertex, 3> a;
        for (int i = 0; i < 3; ++i)
        {
            a[i].pos = verticesBegin[i];
            a[i].u   = static_cast<float>(i) / 3.0f;
            a[i].v   = 0.0f;
        }

        std::array<UVVertex, 3> b;
        for (int i = 0; i < 3; ++i)
        {
            int targetIdx = i;//bestPermutation[i];
            b[i].pos = verticesEnd[targetIdx];
            b[i].u   = static_cast<float>(i) / 3.0f;
            b[i].v   = 1.0f;
        }
/*
        UVVertex a_last = a[0]; a_last.u = 1.0f;
        UVVertex b_last = b[0]; b_last.u = 1.0f;
        3. Выбор диагонали по знаку ориентированного объема складки
        JPH::Vec3 v0 = a[0].pos;
        JPH::Vec3 v1 = a[1].pos;
        JPH::Vec3 v2 = b[0].pos;
        JPH::Vec3 v3 = b[1].pos;

        JPH::Vec3 volEdge1 = v1 - v0;
        JPH::Vec3 volEdge2 = v2 - v0;
        JPH::Vec3 volEdge3 = v3 - v0;

        float volumeSign = volEdge1.Dot(volEdge2.Cross(volEdge3));
        bool useAlternativeDiagonal = (volumeSign < 0.0f);

        --- СИСТЕМНЫЙ ТЕСТ ВЫВЕРНУТОСТИ КОЛЬЦА ЧЕРЕЗ ОБЪЕМ ---
        Считаем объем первого тетраэдра складки (a0, b0, a1) относительно центра пролета
        JPH::Vec3 centerBegin = (verticesBegin[0] + verticesBegin[1] + verticesBegin[2]) / 3.0f;
        JPH::Vec3 centerEnd   = (verticesEnd[0] + verticesEnd[1] + verticesEnd[2]) / 3.0f;
        JPH::Vec3 localPipeCenter = (centerBegin + centerEnd) * 0.5f;

        JPH::Vec3 e0 = a[0].pos - localPipeCenter;
        JPH::Vec3 e1 = b[0].pos - localPipeCenter;
        JPH::Vec3 e2 = a[1].pos - localPipeCenter;

        Если нормали изначально пошли наружу (> 0), взводим флаг инверсии для всего кольца
        bool invertRing = (e0.Dot(e1.Cross(e2)) > 0.0f);

        4. ЧЕСТНАЯ ГЕОМЕТРИЧЕСКАЯ СБОРКА СТЕН (ЖЕСТКИЙ ПАССИВНЫЙ CCW/CW КОНТРОЛЬ)
        useAlternativeDiagonal = true;


                a.u = 1.0f;
        a.v = 1.0f;
        b.u = 0.0f;
        b.v = 1.0f;
        c.u = 0.0f;
        c.v = 0.0f;
        d.u = 1.0f;
        d.v = 0.0f;


*/
        AddPipeWall(meshData, false, a, b);
    }

    // 3. Генерация вершин торца (без изменений)
    Types3d::TriangleVertices MeshBuilder::GenerateTriangleVertices(
        const JPH::RVec3& position, const JPH::RVec3& U, const JPH::RVec3& V, float angleOffset, float radius)
    {
        const float PI = 3.1415926535f;
        Types3d::TriangleVertices vertices;
        for (int i = 0; i < 3; ++i) {
            float angle = (120.0f * i * PI / 180.0f) + angleOffset;
            float x2d = radius * std::cos(angle);
            float y2d = radius * std::sin(angle);
            vertices[i] = position + x2d * U + y2d * V;
        }
        return vertices;
    }
/*
    // Локальная утилита контроля вывернутости индивидуальной боковой стены
    void MeshBuilder::AddOrientedWallTriangle(
        MaxDoF::GameCommon::LevelMeshes::MeshData& meshData,
        const MaxDoF::GameCommon::LevelMeshes::MeshBuilder::UVVertex& p0,
        const MaxDoF::GameCommon::LevelMeshes::MeshBuilder::UVVertex& p1,
        const MaxDoF::GameCommon::LevelMeshes::MeshBuilder::UVVertex& p2,
        const JPH::Vec3& localPipeCenter)
    {
        JPH::Vec3 e0 = p0.pos - localPipeCenter;
        JPH::Vec3 e1 = p1.pos - localPipeCenter;
        JPH::Vec3 e2 = p2.pos - localPipeCenter;

        float volumeSign = e0.Dot(e1.Cross(e2));

        // Возвращаем строгое и чистое сравнение с нулем
        if (volumeSign > 0.0f)
        {
            MaxDoF::GameCommon::LevelMeshes::MeshBuilder::AddTriangle(meshData, p0, p2, p1);
        }
        else
        {
            MaxDoF::GameCommon::LevelMeshes::MeshBuilder::AddTriangle(meshData, p0, p1, p2);
        }
    }
*/
    void MeshBuilder::CreateMeshData(
        MaxDoF::GameCommon::LevelMeshes::MeshData& meshData,
        const MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // Вектора не очищаем (оставляем кумулятивность)

        for(const auto& node : graph.nodes)
        {
            // =========================================================================
            // ЭТАП 1: СБОРКА СЛОЖНОЙ ОБЪЕМНОЙ ГЕОМЕТРИИ (СТЕНЫ ТРУБ)
            // =========================================================================
            if(node.type == NodeType::PipeTunnel)
            {
                if(node.interfaceIds.size() < 2)
                    MaxDoF_LOG_AND_ABORT("bad data")

                // Извлекаем два интерфейса трубы напрямую
                const auto& interface0 = graph.interfaces[node.interfaceIds[0].interfaceId];
                const auto& interface1 = graph.interfaces[node.interfaceIds[1].interfaceId];

                // Натягиваем 6 боковых прямоугольников-складок


                FILE *file = fopen("output.txt", "w");
    
    if (file != NULL) {
        // Записываем данные в файл вместо консоли
        fprintf(file, "x: %f\n", (double)interface0.vertices[0].GetX());
        fprintf(file, "y: %f\n", (double)interface0.vertices[0].GetY());
        fprintf(file, "z: %f\n\n", (double)interface0.vertices[0].GetZ());
        fprintf(file, "x: %f\n", (double)interface0.vertices[1].GetX());
        fprintf(file, "y: %f\n", (double)interface0.vertices[1].GetY());
        fprintf(file, "z: %f\n\n", (double)interface0.vertices[1].GetZ());
        fprintf(file, "x: %f\n", (double)interface0.vertices[2].GetX());
        fprintf(file, "y: %f\n", (double)interface0.vertices[2].GetY());
        fprintf(file, "z: %f\n", (double)interface0.vertices[2].GetZ());
        fprintf(file, "aaaaaaaaa\n");
        fprintf(file, "x: %f\n", (double)interface1.vertices[0].GetX());
        fprintf(file, "y: %f\n", (double)interface1.vertices[0].GetY());
        fprintf(file, "z: %f\n\n", (double)interface1.vertices[0].GetZ());
        fprintf(file, "x: %f\n", (double)interface1.vertices[1].GetX());
        fprintf(file, "y: %f\n", (double)interface1.vertices[1].GetY());
        fprintf(file, "z: %f\n\n", (double)interface1.vertices[1].GetZ());
        fprintf(file, "x: %f\n", (double)interface1.vertices[2].GetX());
        fprintf(file, "y: %f\n", (double)interface1.vertices[2].GetY());
        fprintf(file, "z: %f\n", (double)interface1.vertices[2].GetZ());
        
        // Обязательно закрываем файл
        fclose(file);
    } else {
        printf("Ошибка открытия файла!\n");
    }
                // std::abort();
                ConnectTriangles(meshData, interface0.vertices, interface1.vertices);
            }
            // Для тетраэдра на этом этапе ничего делать не нужно — у него нет боковых складок,
            // его геометрия полностью состоит из плоских граней-интерфейсов!

            // =========================================================================
            // ЭТАП 2: УНИВЕРСАЛЬНАЯ СБОРКА ЗАГЛУШЕК ДЛЯ АБСОЛЮТНО ВСЕХ УЗЛОВ ГРАФА
            // =========================================================================
            // Проходим по всем узлам еще раз и заставляем каждый из них честно
            // проверить свои интерфейсы на наличие глухих тупиков
            for(const auto& orientedInterface : node.interfaceIds)
            {
                const auto& face = graph.interfaces[orientedInterface.interfaceId];

                // Если грань официально ведет в монолитный камень — строим заглушку!
                if(face.nodeB == cNodeSolidRock)
                {
                    MeshBuilder::UVVertex p0{ face.vertices[0], 0.0f, 0.0f };
                    MeshBuilder::UVVertex p1{ face.vertices[1], 1.0f, 0.0f };
                    MeshBuilder::UVVertex p2{ face.vertices[2], 0.5f, 1.0f };

                    // Единое пуленепробиваемое правило для труб, тетраэдров и комнат:
                    if(orientedInterface.inverted)
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
