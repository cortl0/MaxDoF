// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "GameCommon/LevelMeshes/MeshBuilder.h"

#include <array>
#include <cstdint>
#include <random>
#include <vector>


namespace MaxDoF::GameCommon::LevelMeshes
{
    void MeshBuilder::CreateMeshData(GameCommon::LevelMeshes::MeshData& meshData)
    {
        float radius = 5.0f;
        BuildPipe(meshData, JPH::RVec3{0,0,10}, JPH::RVec3{0,0,-10}, radius);
    }

    // 1. Вспомогательная функция для сборки одного треугольника (без изменений)
    void MeshBuilder::AddPhysicalTriangle(
        MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, 
        const UVVertex& p0, 
        const UVVertex& p1, 
        const UVVertex& p2)
    {
        JPH::Vec3 edge1 = p1.pos - p0.pos;
        JPH::Vec3 edge2 = p2.pos - p0.pos;
        JPH::Vec3 normal = edge1.Cross(edge2).Normalized();

        uint32_t currentBase = static_cast<uint32_t>(meshData.vertices.size());
        std::array<UVVertex, 3> pts = { p0, p1, p2 };

        for (int i = 0; i < 3; ++i) 
        {
            MaxDoF::GameCommon::LevelMeshes::Vertex v;
            v.x = pts[i].pos.GetX(); 
            v.y = pts[i].pos.GetY(); 
            v.z = pts[i].pos.GetZ();
            
#ifdef MAXDOF_CLIENT_BUILD
            v.nx = normal.GetX(); 
            v.ny = normal.GetY(); 
            v.nz = normal.GetZ();
            v.u  = pts[i].u; 
            v.v  = pts[i].v;
#endif
            meshData.vertices.push_back(v);
        }

        meshData.indices.push_back(currentBase + 0);
        meshData.indices.push_back(currentBase + 1);
        meshData.indices.push_back(currentBase + 2);
    }

    // 2. ОБНОВЛЕННАЯ ФУНКЦИЯ: Динамический выбор диагонали против схлопывания
    void MeshBuilder::ConnectTriangles(
        MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, 
        const std::array<JPH::Vec3, 3>& verticesBegin, 
        const std::array<JPH::Vec3, 3>& verticesEnd)
    {
        // Комбинаторный поиск кратчайших пар (без изменений)
        const std::array<std::array<int, 3>, 6> permutations = {{
            {0, 1, 2}, {0, 2, 1},
            {1, 0, 2}, {1, 2, 0},
            {2, 0, 1}, {2, 1, 0}
        }};

        float minTotalDistance = FLT_MAX;
        std::array<int, 3> bestPermutation = {0, 1, 2};

        // --- ИСПРАВЛЕННЫЙ БЛОК КОМБИНАТОРНОГО ПОИСКА ---
        for (const auto& p : permutations) {
            // p[0], p[1], p[2] — это конкретные индексы вершин (0, 1 или 2)
            float dist0 = (verticesBegin[0] - verticesEnd[p[0]]).Length();
            float dist1 = (verticesBegin[1] - verticesEnd[p[1]]).Length();
            float dist2 = (verticesBegin[2] - verticesEnd[p[2]]).Length();
            
            float currentSum = dist0 + dist1 + dist2;
            if (currentSum < minTotalDistance) {
                minTotalDistance = currentSum;
                bestPermutation = p;
            }
        }

        // Подготовка базовых UV-вершин
        std::array<UVVertex, 3> a;
        for (int i = 0; i < 3; ++i) {
            a[i].pos = verticesBegin[i];
            a[i].u   = static_cast<float>(i) / 3.0f;
            a[i].v   = 0.0f;
        }

        std::array<UVVertex, 3> b;
        for (int i = 0; i < 3; ++i) {
            int targetIdx = bestPermutation[i];
            b[i].pos = verticesEnd[targetIdx];
            b[i].u   = static_cast<float>(i) / 3.0f; 
            b[i].v   = 1.0f;
        }

        UVVertex a_last = a[0]; a_last.u = 1.0f;
        UVVertex b_last = b[0]; b_last.u = 1.0f;

        // --- АНАЛИТИЧЕСКИЙ ВЫБОР ДИАГОНАЛИ ---
        // Берем 4 вершины первой складки трубы: a0, a1 (вход) и b0, b1 (выход)
        JPH::Vec3 v0 = a[0].pos;
        JPH::Vec3 v1 = a[1].pos;
        JPH::Vec3 v2 = b[0].pos;
        JPH::Vec3 v3 = b[1].pos;

        // Строим три вектора из одной вершины тетраэдра складки
        JPH::Vec3 edge1 = v1 - v0;
        JPH::Vec3 edge2 = v2 - v0;
        JPH::Vec3 edge3 = v3 - v0;

        // Смешанное произведение (Скалярное произведение одного вектора на кросс-продукт двух других)
        // Оно строго определяет знак пространственной ориентации (хиральность) складки
        float volumeSign = edge1.Dot(edge2.Cross(edge3));

        // Если знак отрицательный — геометрия пошла на излом "внутрь" оси (схлопывание). 
        // Принудительно включаем Схему Б, которая расцепит ребра!
        bool useAlternativeDiagonal = (volumeSign < 0.0f);

        // 5. СБОРКА С ДИНАМИЧЕСКИМ ПЕРЕКЛЮЧЕНИЕМ ДИАГОНАЛЕЙ
        if (useAlternativeDiagonal)
        {
            // СХЕМА А (Для положительных углов: расширение через базовую диагональ)
            // Складка 1
            AddPhysicalTriangle(meshData, a[0], b[0], a[1]);
            AddPhysicalTriangle(meshData, b[0], b[1], a[1]);

            // Складка 2
            AddPhysicalTriangle(meshData, a[1], b[1], a[2]);
            AddPhysicalTriangle(meshData, b[1], b[2], a[2]);

            // Складка 3 (Замыкающая)
            AddPhysicalTriangle(meshData, a[2], b[2], a_last);
            AddPhysicalTriangle(meshData, b[2], b_last, a_last);
        }
        else
        {
            // СХЕМА Б (Для отрицательных углов: перенос диагонали, предотвращающий схлопывание!)
            // Переключаем триангуляцию на вторую диагональ четырехугольника: (a0, b0, b1) и (a0, b1, a1)
            
            // Складка 1
            AddPhysicalTriangle(meshData, a[0], b[0], b[1]);
            AddPhysicalTriangle(meshData, a[0], b[1], a[1]);

            // Складка 2
            AddPhysicalTriangle(meshData, a[1], b[1], b[2]);
            AddPhysicalTriangle(meshData, a[1], b[2], a[2]);

            // Складка 3 (Замыкающая)
            AddPhysicalTriangle(meshData, a[2], b[2], b_last);
            AddPhysicalTriangle(meshData, a[2], b_last, a_last);
        }
    }

    // 3. Генерация вершин торца (без изменений)
    std::array<JPH::Vec3, 3> MeshBuilder::GenerateTriangleVertices(
        const JPH::Vec3& position, const JPH::Vec3& U, const JPH::Vec3& V, float angleOffset, float radius)
    {
        const float PI = 3.1415926535f;
        std::array<JPH::Vec3, 3> vertices;
        for (int i = 0; i < 3; ++i) {
            float angle = (120.0f * i * PI / 180.0f) + angleOffset;
            float x2d = radius * std::cos(angle);
            float y2d = radius * std::sin(angle);
            vertices[i] = position + x2d * U + y2d * V;
        }
        return vertices;
    }

    // 4. Очищенная функция с рандомом (без изменений)
    void MeshBuilder::BuildPipe(MaxDoF::GameCommon::LevelMeshes::MeshData& meshData, const JPH::RVec3& begin, const JPH::RVec3& end, float radius)
    {
        const float PI = 3.1415926535f;

        JPH::Vec3 pBegin(begin);
        JPH::Vec3 pEnd(end);
        
        JPH::Vec3 dir = pEnd - pBegin;
        float length = dir.Length();
        if (length < 1e-4f) return;
        dir /= length;

        JPH::Vec3 up = (std::abs(dir.GetY()) > 0.99f) ? JPH::Vec3(0.0f, 0.0f, 1.0f) : JPH::Vec3(0.0f, 1.0f, 0.0f);
        JPH::Vec3 U = dir.Cross(up).Normalized();
        JPH::Vec3 V = dir.Cross(U);

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 2.0f * PI);

        float randomAngleBegin = dis(gen);
        float randomAngleEnd   = dis(gen);

        std::array<JPH::Vec3, 3> verticesBegin = GenerateTriangleVertices(pBegin, U, V, randomAngleBegin, radius);
        std::array<JPH::Vec3, 3> verticesEnd   = GenerateTriangleVertices(pEnd, U, V, randomAngleEnd, radius);

        ConnectTriangles(meshData, verticesBegin, verticesEnd);
    }
}
