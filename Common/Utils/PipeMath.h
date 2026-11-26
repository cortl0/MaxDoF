// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once


#include <Jolt/Jolt.h>

#include "Common/Types/LevelGraph.h"
#include "Common/Types/Poses.h"

namespace MaxDoF::Common::Utils
{
    Types::PhysPose CalculatePoseFromVertices(const Types::Level::Graph::Triangle::Vertices &vertices);
    JPH::Vec3 RVec3_To_Vec3(const JPH::RVec3&);
    JPH::RVec3 Vec3_To_RVec3(const JPH::Vec3 &p);
}
