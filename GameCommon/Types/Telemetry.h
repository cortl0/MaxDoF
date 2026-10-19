// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameCommon_Types_Telemetry_H_
#define MaxDoF_GameCommon_Types_Telemetry_H_

namespace MaxDoF::GameCommon::Types
{
    struct Telemetry
    {
        struct Vector
        {
            float x;
            float y;
            float z;
        };
        Vector position;
        Vector direction;
        Vector localVelocity;
        Vector localAngularVelocity;
    };
}  // namespace MaxDoF::GameCommon::Types

#endif
