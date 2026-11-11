// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "PipeGraphGenerator.h"

#include "Common/LevelMeshes/MeshBuilder.h"
#include "MeshBuilder.h"
#include "Pipe.h"

namespace MaxDoF::Common::LevelGraphs
{
    void PipeGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::Common::LevelGraphs;

        // 1. КОНСТАНТЫ СТРОГОЙ ГЕОМЕТРИИ ТРУБЫ
        const float cPipeRadius = 1.732f*2;     // Радиус сечения туннеля для 1м корабля
        const float cPipeLength = 50.0f;      // Длина трубы (строго >= 1.0м по закону 1 метра)

        // Позиции торцов: труба центрирована, идет из Z = 5.0м (сзади) в Z = -5.0м (вперед, вглубь экрана)
        JPH::RVec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::RVec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);

        Pipe pipe;
        pipe.SetVetricesAndInterfaces(pBegin, pEnd, cPipeRadius);
        pipe.FillGraph(graph, {false, true}, true, {Types::LevelGraphs::InterfaceType::SolidRock, Types::LevelGraphs::InterfaceType::SolidRock});
    }
}
