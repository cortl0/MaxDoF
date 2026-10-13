// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_Types_Converters_H_
#define MaxDoF_Types_Converters_H_

#include <Jolt/Jolt.h>

namespace MaxDoF::Types
{
    inline const JPH::RVec3 cUnitScaleVector{ 1.0f, 1.0f, 1.0f };
    inline const JPH::RVec3 cZeroVector = JPH::RVec3::sZero();
    inline const JPH::Quat cIdentity = JPH::Quat::sIdentity();

    struct Pose
    {
        JPH::RVec3 position;
        JPH::Quat orientation;
    };

    inline const Pose cDefaultPose
    {
        .position = cZeroVector,
        .orientation = cIdentity
    };
}  // namespace MaxDoF::Types

#endif
