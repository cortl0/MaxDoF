// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "DeathmatchGraphGenerator.h"
#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Tetrahedron.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void DeathmatchGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // 1. РАДИУС НАШЕЙ ЕДИНСТВЕННОЙ КОМНАТЫ
        const float cArenaRadius = 50.0f;

        Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(cArenaRadius);

        // 1. КОНСТАНТЫ СТРОГОЙ ГЕОМЕТРИИ
        const float cPipeLength = 50.0f;

        // 4 уникальные опорные точки залы (Верхушка купола сзади)
        auto arenaSharedVertices = tetrahedron.GetVertices();

        // 2. СОЗДАЕМ ЦЕНТРАЛЬНЫЙ УЗЕЛ-АРЕНУ
        VolumeNode arenaNode;
        arenaNode.id = static_cast<NodeKey>(graph.nodes.size());
        arenaNode.type = NodeType::Tetrahedron;

        
        auto faceLayout = tetrahedron.GetInterfaces();

        // ID нашей единственной трубы
        const NodeKey targetPipeId = 1;

        // =========================================================================
        // ГЕНЕРИРУЕМ ИНТЕРФЕЙСЫ ЦЕНТРАЛЬНОГО ЗАЛА
        // =========================================================================
        for (int i = 0; i < 4; ++i)
        {
            const auto& indices = faceLayout[i];
            
            TriangleInterface face;
            face.id = static_cast<TriangleInterfaceKey>(graph.interfaces.size());
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

        // Извлекаем интерфейс переднего щита (он был сгенерирован последним, под индексом 3)
        auto innerInterface = graph.interfaces[3];
        // auto tempI = innerInterface.vertices[1];
        // innerInterface.vertices[1] = innerInterface.vertices[2];
        // innerInterface.vertices[2] = tempI;

        // Направление трубы — строго вперед вдоль отрицательного Z (вектор взгляда)
        JPH::Vec3 pipeDir(0.0f, 0.0f, -1.0f);


        // --- ВНЕШНИЙ ШЛЮЗ ТРУБЫ (Выход в космос, глухой торец) ---
        TriangleInterface outerInterface;
        outerInterface.id = static_cast<TriangleInterfaceKey>(graph.interfaces.size());
        outerInterface.nodeA = pipeNode.id;
        outerInterface.nodeB = cNodeAsteroidSurface;

        // Генерируем вершины внешнего торца скрученными на 30 градусов
        //outerInterface.vertices = GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(exitCenter, U, V, cFixedRotation, gateSize);
        outerInterface.vertices = innerInterface.vertices;
        outerInterface.vertices[0] += pipeDir * cPipeLength;
        outerInterface.vertices[1] += pipeDir * cPipeLength;
        outerInterface.vertices[2] += pipeDir * cPipeLength;
        // auto tempO = outerInterface.vertices[1];
        // outerInterface.vertices[1] = outerInterface.vertices[2];
        // outerInterface.vertices[2] = tempO;
        graph.interfaces.push_back(outerInterface);

        // СВЯЗЫВАЕМ ТОРЦЫ С НАШИМ КОРИДОРОМ:
        // 1. Стык с залой (innerInterface): заходит со встречной фазой -> inverted = false (0)
        pipeNode.interfaceIds.push_back({ innerInterface.id, false });
        
        // 2. Выход в космос (outerInterface): изначально смотрит наружу -> inverted = true (1), разворачиваем внутрь!
        pipeNode.interfaceIds.push_back({ outerInterface.id, false });

        // Пушим трубу в граф уровня
        graph.nodes.push_back(pipeNode);
    }
}
