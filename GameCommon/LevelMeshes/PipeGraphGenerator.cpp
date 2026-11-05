// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "PipeGraphGenerator.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    void PipeGraphGenerator::GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        graph.nodes.clear();
        graph.interfaces.clear();

        // 1. КОНСТАНТЫ СТРОГОЙ ГЕОМЕТРИИ ТРУБЫ
        const float cPipeRadius = 1.732f*2;     // Радиус сечения туннеля для 1м корабля
        const float cPipeLength = 50.0f;      // Длина трубы (строго >= 1.0м по закону 1 метра)
        const float cFixedRotation = 3.1415926535f / 6.0f; // Угол скручивания 30 градусов (PI / 6)

        // Позиции торцов: труба центрирована, идет из Z = 5.0м (сзади) в Z = -5.0м (вперед, вглубь экрана)
        JPH::Vec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::Vec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);

        // Стабильный базис плоскости сечения трубы (X-вправо, Y-вверх)
        JPH::Vec3 U(1.0f, 0.0f, 0.0f);
        JPH::Vec3 V(0.0f, 1.0f, 0.0f);

        // 2. СОЗДАЕМ ОДИН ОБЪЕМНЫЙ УЗЕЛ-ТРУБУ
        VolumeNode pipeNode;
        pipeNode.id = static_cast<uint32_t>(graph.nodes.size());
        pipeNode.type = NodeType::PipeTunnel;
        pipeNode.sizeParameter = cPipeRadius;

        // --- ВХОДНОЙ ТОРЕЦ (Глухой интерфейс на Z = +5.0м) ---
        TriangleInterface innerInterface;
        innerInterface.id = static_cast<uint32_t>(graph.interfaces.size());
        innerInterface.nodeA = pipeNode.id;       // Труба является родителем
        innerInterface.nodeB = cNodeSolidRock;   // Заглушка, глухой камень

        // Генерируем вершины начального треугольника (базовый угол 0.0f)
        innerInterface.vertices = GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(pBegin, U, V, 0.0f, cPipeRadius);
        graph.interfaces.push_back(innerInterface);

        // --- ВЫХОДНОЙ ТОРЕЦ (Глухой интерфейс на Z = -5.0м) ---
        TriangleInterface outerInterface;
        outerInterface.id = static_cast<uint32_t>(graph.interfaces.size());
        outerInterface.nodeA = pipeNode.id;       // Труба является родителем
        outerInterface.nodeB = cNodeSolidRock;   // Заглушка, глухой камень

        // Генерируем вершины конечного треугольника, закрученные ровно на 30 градусов
        outerInterface.vertices = GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(pEnd, U, V, cFixedRotation, cPipeRadius);
        graph.interfaces.push_back(outerInterface);

        // Связываем интерфейсы с трубой.
        // По нашему правилу "наоборот": так как труба сама породила оба этих ребра,
        // для неё оба интерфейса сонаправлены с ней (нормали смотрят внутрь трубы).
        // Поэтому для ОБОИХ интерфейсов ставим флаг inverted = true!
        bool inverted = true;
        pipeNode.interfaceIds.push_back({ innerInterface.id, inverted });
        inverted = false;
        pipeNode.interfaceIds.push_back({ outerInterface.id, inverted });

        // Пушим готовую изолированную трубу в список узлов уровня
        graph.nodes.push_back(pipeNode);
    }
}
