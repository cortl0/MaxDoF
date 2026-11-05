// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "DeathmatchGraphGenerator.h"
#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include <cmath>

namespace MaxDoF::GameCommon::LevelGraphs
{
    void DeathmatchGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        graph.nodes.clear();
        graph.interfaces.clear();

        // 1. КОНСТАНТЫ СТРОГОЙ ГЕОМЕТРИИ
        const float cArenaRadius = 50.0f;     
        const float cPipeLength = 50.0f;      
        const float cFixedRotation = -3.1415926535f / 3.0f; // 30 градусов

        const float sqrt3 = 1.7320508f;
        const float sqrt6 = 2.4494897f;

        const float edgeLength = (4.0f / sqrt6) * cArenaRadius;
        const float gateSize = edgeLength / sqrt3;

        const float zBack = cArenaRadius;            
        const float zFront = -cArenaRadius / 3.0f;    
        const float yTop = gateSize;                  
        const float yBottom = -gateSize / 2.0f;       

        // 4 уникальные опорные точки залы (Верхушка купола сзади)
        std::array<JPH::Vec3, 4> arenaSharedVertices = {{
            JPH::Vec3(0.0f, 0.0f, zBack),                     // Вершина 0 (Сзади, +Z)
            JPH::Vec3(0.0f, yTop, zFront),                    // Вершина 1 (Вперед-вверх)
            JPH::Vec3(-edgeLength / 2.0f, yBottom, zFront),   // Вершина 2 (Вперед-влево-вниз)
            JPH::Vec3(edgeLength / 2.0f, yBottom, zFront)     // Вершина 3 (Вперед-вправо-вниз)
        }};

        // 2. СОЗДАЕМ ЦЕНТРАЛЬНЫЙ УЗЕЛ-АРЕНУ
        VolumeNode arenaNode;
        arenaNode.id = static_cast<uint32_t>(graph.nodes.size());
        arenaNode.type = NodeType::Tetrahedron;
        arenaNode.sizeParameter = cArenaRadius;

        std::array<std::array<int, 3>, 4> faceLayout = {{
            {0, 2, 1}, // Грань 0 (Левая стена)
            {0, 1, 3}, // Грань 1 (Правая стена)
            {0, 3, 2}, // Грань 2 (Нижняя стена/Пол)
            {1, 2, 3}  // Грань 3 (Передний треугольный щит прямо перед глазами на -Z)
        }};

        // ID нашей единственной трубы
        const uint32_t targetPipeId = 1;

        // =========================================================================
        // ГЕНЕРИРУЕМ ИНТЕРФЕЙСЫ ЦЕНТРАЛЬНОГО ЗАЛА
        // =========================================================================
        for (int i = 0; i < 4; ++i)
        {
            const auto& indices = faceLayout[i];
            
            TriangleInterface face;
            face.id = static_cast<uint32_t>(graph.interfaces.size());
            face.nodeA = arenaNode.id;

            // Хирургическое разделение тупика и коридора
            if (i < 3)
            {
                face.nodeB = cNodeSolidRock; // Глухой камень
            }
            else
            {
                face.nodeB = targetPipeId;   // Соединение с трубой
            }

            // ИСПРАВЛЕНО: Честный покомпонентный перенос вершин по индексам 0, 1, 2
            face.vertices[0] = arenaSharedVertices[indices[0]];
            face.vertices[1] = arenaSharedVertices[indices[1]];
            face.vertices[2] = arenaSharedVertices[indices[2]];
            
            graph.interfaces.push_back(face);

            // Тетраэдр изначально правый — инверсия заглушек не нужна
            const bool arenaInverted = false;
            arenaNode.interfaceIds.push_back({ face.id, arenaInverted });
        }

        // Пушим готовую залу в граф
        graph.nodes.push_back(arenaNode);
        // =========================================================================
        // 3. ГЕНЕРИРУЕМ ЕДИНСТВЕННЫЙ УЗЕЛ-ТРУБУ НА ОСИ -Z
        // =========================================================================
        VolumeNode pipeNode;
        pipeNode.id = targetPipeId;
        pipeNode.type = NodeType::PipeTunnel;
        pipeNode.sizeParameter = gateSize;

        // Извлекаем интерфейс переднего щита (он был сгенерирован последним, под индексом 3)
        const auto& innerInterface = graph.interfaces[3];

        // Направление трубы — строго вперед вдоль отрицательного Z (вектор взгляда)
        JPH::Vec3 pipeDir(0.0f, 0.0f, -1.0f);

        // Базис плоскости сечения (X-вправо, Y-вверх)
        JPH::Vec3 U(1.0f, 0.0f, 0.0f);
        JPH::Vec3 V(0.0f, 1.0f, 0.0f);

        // ИСПРАВЛЕНО: Честное покомпонентное извлечение вершин треугольника по индексам 0, 1, 2
        JPH::Vec3 faceCenter = (innerInterface.vertices[0] + innerInterface.vertices[1] + innerInterface.vertices[2]) / 3.0f;
        
        // Позиция внешнего шлюза трубы в глубоком космосе (на 10 метров дальше по оси -Z)
        JPH::Vec3 exitCenter = faceCenter + pipeDir * cPipeLength;

        // --- ВНЕШНИЙ ШЛЮЗ ТРУБЫ (Выход в космос, глухой торец) ---
        TriangleInterface outerInterface;
        outerInterface.id = static_cast<uint32_t>(graph.interfaces.size());
        outerInterface.nodeA = pipeNode.id;
        outerInterface.nodeB = cNodeOpenSpace;

        // Генерируем вершины внешнего торца скрученными на 30 градусов
        outerInterface.vertices = GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(exitCenter, U, V, cFixedRotation, gateSize);
        graph.interfaces.push_back(outerInterface);

        // СВЯЗЫВАЕМ ТОРЦЫ С НАШИМ КОРИДОРОМ:
        // 1. Стык с залой (innerInterface): заходит со встречной фазой -> inverted = false (0)
        pipeNode.interfaceIds.push_back({ innerInterface.id, false });
        
        // 2. Выход в космос (outerInterface): изначально смотрит наружу -> inverted = true (1), разворачиваем внутрь!
        pipeNode.interfaceIds.push_back({ outerInterface.id, true });

        // Пушим трубу в граф уровня
        graph.nodes.push_back(pipeNode);
    }
}
