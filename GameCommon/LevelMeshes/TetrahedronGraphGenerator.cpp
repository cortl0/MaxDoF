// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "TetrahedronGraphGenerator.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "TetrahedronGraphGenerator.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void TetrahedronGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        graph.nodes.clear();
        graph.interfaces.clear();

        // 1. РАДИУС НАШЕЙ ЕДИНСТВЕННОЙ КОМНАТЫ
        const float cArenaRadius = 50.0f;

        const float sqrt3 = 1.7320508f;
        const float sqrt6 = 2.4494897f;

        // Истинная длина ребра правильного тетраэдра при заданном радиусе описанной сферы
        const float edgeLength = (4.0f / sqrt6) * cArenaRadius;
        // Радиус описанной окружности грани (размер прохода трубы)
        const float gateSize = edgeLength / sqrt3;

        // Расчет пространственного положения плоскостей для Z-ориентированного тетраэдра
        const float zBack = cArenaRadius;            // Одиночная вершина сзади (в плюсе по Z)
        const float zFront = -cArenaRadius / 3.0f;    // Плоскость основания впереди (в минусе по Z)

        // Высоты для правильного треугольника основания на фронтальной плоскости XY
        const float yTop = gateSize;                  // Верхняя точка основания (смотрит вверх по Y)
        const float yBottom = -gateSize / 2.0f;       // Нижняя линия основания

        // 4 уникальные опорные точки — ВЕРШИНА СЗАДИ (+Z), ОСНОВАНИЕ ВПЕРЕДИ (-Z)
        std::array<JPH::Vec3, 4> arenaSharedVertices = {{
            JPH::Vec3(0.0f, 0.0f, zBack),                     // Вершина 0 (Сзади, на оси +Z)
            JPH::Vec3(0.0f, yTop, zFront),                    // Вершина 1 (Вперед-вверх)
            JPH::Vec3(-edgeLength / 2.0f, yBottom, zFront),   // Вершина 2 (Вперед-влево-вниз)
            JPH::Vec3(edgeLength / 2.0f, yBottom, zFront)     // Вершина 3 (Вперед-вправо-вниз)
        }};

        // 2. СОЗДАЕМ ОДИН ОБЪЕМНЫЙ УЗЕЛ-ТЕТРАЭДР
        VolumeNode arenaNode;
        arenaNode.id = static_cast<uint32_t>(graph.nodes.size());
        arenaNode.type = NodeType::Tetrahedron;
        arenaNode.sizeParameter = cArenaRadius;

        // Индексы вершин для сборки 4-х граней тетраэдра
        // Обход (Winding) гарантирует нормали строго ВНУТРЬ этой единственной комнаты
        std::array<std::array<int, 3>, 4> faceLayout = {{
            {0, 2, 1}, // Грань 0 (Левая стена)
            {0, 1, 3}, // Грань 1 (Правая стена)
            {0, 3, 2}, // Грань 2 (Нижняя стена)
            {1, 2, 3}  // Грань 3 (Переднияя стена перед глазами)
        }};

        // Генерируем 4 абсолютно глухие грани-интерфейсы
        for (int i = 0; i < 4; ++i)
        {
            const auto& indices = faceLayout[i];

            TriangleInterface face;
            face.id = static_cast<uint32_t>(graph.interfaces.size());
            face.nodeA = arenaNode.id;
            face.nodeB = cNodeSolidRock;

            face.vertices[0] = arenaSharedVertices[indices[0]];
            face.vertices[1] = arenaSharedVertices[indices[1]];
            face.vertices[2] = arenaSharedVertices[indices[2]];

            graph.interfaces.push_back(face);

            const bool inverted = false;
            arenaNode.interfaceIds.push_back({ face.id, inverted });
        }

        graph.nodes.push_back(arenaNode);
    }
}
