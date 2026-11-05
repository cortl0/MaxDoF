// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "TetrahedronGraphGenerator.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "TetrahedronGraphGenerator.h"
#include "Tetrahedron.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void TetrahedronGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // 1. РАДИУС НАШЕЙ ЕДИНСТВЕННОЙ КОМНАТЫ
        const float cArenaRadius = 50.0f;

        Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(cArenaRadius);

        // 4 уникальные опорные точки — ВЕРШИНА СЗАДИ (+Z), ОСНОВАНИЕ ВПЕРЕДИ (-Z)
        auto arenaSharedVertices = tetrahedron.GetVertices();

        // 2. СОЗДАЕМ ОДИН ОБЪЕМНЫЙ УЗЕЛ-ТЕТРАЭДР
        VolumeNode arenaNode;
        arenaNode.id = static_cast<NodeKey>(graph.nodes.size());
        arenaNode.type = NodeType::Tetrahedron;

        // Индексы вершин для сборки 4-х граней тетраэдра
        // Обход (Winding) гарантирует нормали строго ВНУТРЬ этой единственной комнаты
        auto faceLayout = tetrahedron.GetInterfaces();

        // Генерируем 4 абсолютно глухие грани-интерфейсы
        for (int i = 0; i < 4; ++i)
        {
            const auto& indices = faceLayout[i];

            TriangleInterface face;
            face.id = static_cast<TriangleInterfaceKey>(graph.interfaces.size());
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
