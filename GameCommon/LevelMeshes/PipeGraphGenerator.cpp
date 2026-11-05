// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "PipeGraphGenerator.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Pipe.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void PipeGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // 1. КОНСТАНТЫ СТРОГОЙ ГЕОМЕТРИИ ТРУБЫ
        const float cPipeRadius = 1.732f*2;     // Радиус сечения туннеля для 1м корабля
        const float cPipeLength = 50.0f;      // Длина трубы (строго >= 1.0м по закону 1 метра)

        // Позиции торцов: труба центрирована, идет из Z = 5.0м (сзади) в Z = -5.0м (вперед, вглубь экрана)
        JPH::RVec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::RVec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);

        Pipe pipe;
        pipe.SetVetricesAndInterfaces(pBegin, pEnd, cPipeRadius);

        // 2. СОЗДАЕМ ОДИН ОБЪЕМНЫЙ УЗЕЛ-ТРУБУ
        VolumeNode pipeNode;
        pipeNode.id = static_cast<NodeKey>(graph.nodes.size());
        pipeNode.type = NodeType::PipeTunnel;

        // --- ВХОДНОЙ ТОРЕЦ (Глухой интерфейс на Z = +5.0м) ---
        TriangleInterface firstInterface;
        firstInterface.id = static_cast<TriangleInterfaceKey>(graph.interfaces.size());
        firstInterface.nodeA = pipeNode.id;       // Труба является родителем
        firstInterface.nodeB = cNodeSolidRock;   // Заглушка, глухой камень

        const auto& vertices = pipe.GetVertices();
        // Генерируем вершины начального треугольника (базовый угол 0.0f)
        firstInterface.vertices = vertices.front();
        graph.interfaces.push_back(firstInterface);

        TriangleInterface secondInterface;
        secondInterface.id = static_cast<TriangleInterfaceKey>(graph.interfaces.size());
        secondInterface.nodeA = pipeNode.id;       // Труба является родителем
        secondInterface.nodeB = cNodeSolidRock;   // Заглушка, глухой камень

        secondInterface.vertices = vertices.back();
        graph.interfaces.push_back(secondInterface);

        // Связываем интерфейсы с трубой.
        // По нашему правилу "наоборот": так как труба сама породила оба этих ребра,
        // для неё оба интерфейса сонаправлены с ней (нормали смотрят внутрь трубы).
        // Поэтому для ОБОИХ интерфейсов ставим флаг inverted = true!
        bool inverted = false;
        pipeNode.interfaceIds.push_back({ firstInterface.id, inverted });
        inverted = true;
        pipeNode.interfaceIds.push_back({ secondInterface.id, inverted });

        // Пушим готовую изолированную трубу в список узлов уровня
        graph.nodes.push_back(pipeNode);
    }
}
