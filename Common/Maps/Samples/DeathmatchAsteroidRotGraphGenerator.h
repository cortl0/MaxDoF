// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraph.h"

namespace MaxDoF::Common::Maps
{
    class DeathmatchAsteroidRotGraphGenerator
    {
    public:
        DeathmatchAsteroidRotGraphGenerator() = delete;

        static void GenerateGraph(Common::Types::Level::Graph&, JPH::RVec3 offset);
    };
}
