// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Utils/PipeMath.h"

#include <cmath>
#include <algorithm>
#include <random>

#include <Jolt/Jolt.h>

namespace MaxDoF::Common::Utils
{
    JPH::Vec3 RVec3_To_Vec3(const JPH::RVec3& p)
    {
        return
        {
            static_cast<float>(p.GetX()),
            static_cast<float>(p.GetY()),
            static_cast<float>(p.GetZ())
        };
    }

    JPH::RVec3 Vec3_To_RVec3(const JPH::Vec3& p)
    {
        return
        {
            static_cast<double>(p.GetX()),
            static_cast<double>(p.GetY()),
            static_cast<double>(p.GetZ())
        };
    }
}
