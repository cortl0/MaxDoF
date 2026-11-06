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
    void TetrahedronGraphGenerator::GenerateGraph(GameCommon::Types::LevelGraphs::PureLevelGraph& graph)
    {
        using namespace MaxDoF::GameCommon::LevelGraphs;

        // 1. РАДИУС НАШЕЙ ЕДИНСТВЕННОЙ КОМНАТЫ
        const float cArenaRadius = 50.0f;

        Tetrahedron tetrahedron;
        tetrahedron.SetVetricesAndInterfaces(cArenaRadius);
        tetrahedron.FillGraph(graph);
    }
}
