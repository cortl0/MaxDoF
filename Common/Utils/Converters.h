// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_Common_Utils_Converters_H_
#define MaxDoF_Common_Utils_Converters_H_

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

namespace MaxDoF::Common::Utils
{
    JPH::Vec3 getLocalAngularVelocities(const JPH::Quat &qPrev, const JPH::Quat &qCurr, float dt);
    JPH::Vec3 getLocalLinearVelocities(const JPH::Vec3 &pPrev, const JPH::Vec3 &pCurr, const JPH::Quat &qCurr, float dt);
} // namespace MaxDoF::Common::Utils

#endif
