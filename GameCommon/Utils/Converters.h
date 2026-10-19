// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameCommon_Utils_Converters_H_
#define MaxDoF_GameCommon_Utils_Converters_H_

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

namespace MaxDoF::GameCommon::Utils
{
    JPH::Vec3 getLocalAngularVelocities(const JPH::Quat &qPrev, const JPH::Quat &qCurr, float dt);
    JPH::Vec3 getLocalLinearVelocities(const JPH::Vec3 &pPrev, const JPH::Vec3 &pCurr, const JPH::Quat &qCurr, float dt);
} // namespace MaxDoF::GameCommon::Utils

#endif
