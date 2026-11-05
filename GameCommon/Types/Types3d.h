// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
// #include <Jolt/Math/Quat.h>
#include <Jolt/Math/Real.h>

namespace MaxDoF::GameCommon::Types3d
{
    using TriangleVertices = std::array<JPH::RVec3, 3>;
    using TetrahedronVertices = std::array<JPH::RVec3, 4>;
    using Interfaces = std::array<std::array<int, 3>, 4>;
}
