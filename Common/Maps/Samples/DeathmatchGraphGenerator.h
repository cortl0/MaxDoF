// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraphStructures.h"

namespace MaxDoF::Common::Maps
{
    class DeathmatchGraphGenerator
    {
    public:
        DeathmatchGraphGenerator() = delete;

        static void GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset);
    };
}
