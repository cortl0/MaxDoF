// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#ifndef MaxDoF_Common_Types_Telemetry_H_
#define MaxDoF_Common_Types_Telemetry_H_

namespace MaxDoF::Common::Types
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
}  // namespace MaxDoF::Common::Types

#endif
