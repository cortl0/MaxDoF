// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Meshes/Pipe.h"
#include "Common/Types/LevelGraph.h"

namespace MaxDoF::Common::Maps
{
    class PipeGraphGenerator
    {
    public:
        PipeGraphGenerator() = delete;

        static void GenerateGraph(
            Types::Level::Graph&,
            JPH::RVec3 halfSize,
            JPH::Quat endRotation,
            const LevelGraphs::Pipe::Config&
        );
    };
}

