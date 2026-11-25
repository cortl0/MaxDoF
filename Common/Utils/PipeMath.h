// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/Poses.h"

namespace MaxDoF::Common::Utils
{
    int ChooseOptimalPose(const Types::PhysPose& pose1, const Types::PhysPose& pose2, const Types::PhysPose& pose3);
    float getRandomFloat(float min, float max);
    JPH::Quat getRandomEulerRotation30Deg();
    JPH::Vec3 RVec3_To_Vec3(const JPH::RVec3&);
    JPH::RVec3 Vec3_To_RVec3(const JPH::Vec3 &p);
    JPH::Quat rotateLocal(const JPH::Quat &base_rot, const JPH::Quat &delta_rot);
    JPH::Quat rotateGlobal(const JPH::Quat &base_rot, const JPH::Quat &delta_rot);
}
