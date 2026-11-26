// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraph.h"

namespace MaxDoF::Common::Maps
{
    class RaceGenerator
    {
    public:
        RaceGenerator() = delete;

        static void GenerateGraph
        (
            Types::Level::Graph&,
            const Types::Level::Graph::Triangle::Key beginTriangleKey,
            const Types::Level::Graph::Triangle::Key endTriangleKey
        );

        static void GenerateGraph(Types::Level::Graph&);
    };
}
