// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Utils/PipeMath.h"

#include <cmath>
#include <algorithm>

namespace MaxDoF::Common::Utils
{
    inline JPH::RVec3 GetForward(const JPH::Quat& q) {
        return {
            1.0 - 2.0 * (q.GetY() * q.GetY() + q.GetZ() * q.GetZ()),
            2.0 * (q.GetX() * q.GetY() + q.GetW() * q.GetZ()),
            2.0 * (q.GetX() * q.GetZ() - q.GetW() * q.GetY())
        };
    }

    double TunnelCost(const Types::PhysPose& start, const Types::PhysPose& target) {
        auto diff{target.position - start.position};
        double dist = std::sqrt(diff.GetX() * diff.GetX() + diff.GetY() * diff.GetY() + diff.GetZ() * diff.GetZ());

        double qDot = std::abs(
            start.orientation.GetW() * target.orientation.GetW() + start.orientation.GetX() * target.orientation.GetX() +
            start.orientation.GetY() * target.orientation.GetY() + start.orientation.GetZ() * target.orientation.GetZ());

        double fullAngle = 2.0 * std::acos(std::clamp(qDot, 0.0, 1.0));

        if (dist < 1e-6)
            return (fullAngle * fullAngle) / 6.0;

        JPH::RVec3 dir{ diff.GetX() / dist, diff.GetY() / dist, diff.GetZ() / dist };
        auto f0 = GetForward(start.orientation);
        auto f1 = GetForward(target.orientation);

        double a0 = std::acos(std::clamp(f0.GetX() * dir.GetX() + f0.GetY() * dir.GetY() + f0.GetZ() * dir.GetZ(), -1.0, 1.0));
        double a1 = std::acos(std::clamp(f1.GetX() * dir.GetX() + f1.GetY() * dir.GetY() + f1.GetZ() * dir.GetZ(), -1.0, 1.0));
        double bend3D = std::acos(std::clamp(f0.GetX() * f1.GetX() + f0.GetY() * f1.GetY() + f0.GetZ() * f1.GetZ(), -1.0, 1.0));

        double spiralFactor = (a0 * a0 + a1 * a1 + 0.5 * bend3D * bend3D) / 6.0;
        double effectiveDist = dist * (1.0 + spiralFactor);
        double totalAngle = std::max({a0 + a1, bend3D, fullAngle});

        return (effectiveDist * effectiveDist) + (totalAngle * totalAngle) / 6.0;
    }

    int ChooseOptimalPose(const Types::PhysPose& pose1, const Types::PhysPose& pose2, const Types::PhysPose& pose3) {
        return (TunnelCost(pose2, pose1) <= TunnelCost(pose3, pose1)) ? 2 : 3;
    }
}
