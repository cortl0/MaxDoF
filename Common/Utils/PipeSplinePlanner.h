// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>
#include <Jolt/Jolt.h>
#include "Common/Types/Poses.h"

namespace MaxDoF::Common::Utils
{
    class PipeSplinePlanner
    {
    public:
        struct Params
        {
            float pipeDiameter      = 1.0f;  // Диаметр трубы (м)
            float maxTurnDeg        = 12.0f; // Макс. угол изгиба между сегментами (градусы)
            float maxTwistPerSegDeg = 10.0f; // Макс. закрутка крена на сегмент (градусы)
        };

        struct Result
        {
            std::vector<Types::PhysPose> poses; // Равномерно расставленные позы сечений
            double optimalSegmentLength = 0.0;  // Рассчитанная единая длина сегмента
            double totalLength          = 0.0;  // Полная длина оси трубы
            bool   isClosedLoop         = false;// Флаг замкнутого контура (start == end)
        };

        // Проектирует гладкую 3D-трассу (S-кривую, спираль или замкнутое кольцо)
        // с единой оптимальной длиной сегмента и точной стыковкой крена
        static Result Plan(
            const Types::PhysPose& startPose,
            const Types::PhysPose& endPose,
            const Params& params
        );
    };
}
