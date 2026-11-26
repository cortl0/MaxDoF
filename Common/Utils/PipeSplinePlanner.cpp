// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Utils/PipeSplinePlanner.h"

#include <cmath>
#include <algorithm>
#include "Common/Utils/PipeMath.h"

namespace MaxDoF::Common::Utils
{
    namespace
    {
        JPH::RVec3 EvalHermitePos(
            const JPH::RVec3& p0, const JPH::RVec3& t0,
            const JPH::RVec3& p1, const JPH::RVec3& t1,
            double t)
        {
            double t2 = t * t;
            double t3 = t2 * t;
            double h00 =  2.0 * t3 - 3.0 * t2 + 1.0;
            double h10 =        t3 - 2.0 * t2 + t;
            double h01 = -2.0 * t3 + 3.0 * t2;
            double h11 =        t3 -       t2;

            return p0 * h00 + t0 * h10 + p1 * h01 + t1 * h11;
        }

        JPH::RVec3 EvalHermiteTangent(
            const JPH::RVec3& p0, const JPH::RVec3& t0,
            const JPH::RVec3& p1, const JPH::RVec3& t1,
            double t)
        {
            double t2 = t * t;
            double dh00 =  6.0 * t2 - 6.0 * t;
            double dh10 =  3.0 * t2 - 4.0 * t + 1.0;
            double dh01 = -6.0 * t2 + 6.0 * t;
            double dh11 =  3.0 * t2 - 2.0 * t;

            return p0 * dh00 + t0 * dh10 + p1 * dh01 + t1 * dh11;
        }

        struct SplineSpanData
        {
            JPH::RVec3 pA, tA, pB, tB;
            JPH::Quat  rotA, rotB;
            double     arcLength = 0.0;
            double     maxCurvature = 0.0;
            float      totalTwistAngle = 0.0f;
            std::vector<double> arcTable;
        };

        SplineSpanData AnalyzeSpan(
            const Types::PhysPose& poseA,
            const Types::PhysPose& poseB,
            double minBendRadius,
            int samples = 512)
        {
            SplineSpanData data;
            data.pA = poseA.position;
            data.pB = poseB.position;
            data.rotA = poseA.orientation;
            data.rotB = poseB.orientation;

            JPH::RVec3 dirA = Vec3_To_RVec3(-poseA.orientation.RotateAxisZ());
            JPH::RVec3 dirB = Vec3_To_RVec3(-poseB.orientation.RotateAxisZ());

            double distAB = (data.pB - data.pA).Length();
            double tangentScale = std::max(distAB * 1.15, minBendRadius * 2.5);

            data.tA = dirA * tangentScale;
            data.tB = dirB * tangentScale;

            data.arcTable.resize(samples + 1, 0.0);
            JPH::RVec3 prevPt = data.pA;
            JPH::Vec3 prevTan = -poseA.orientation.RotateAxisZ();
            JPH::Quat transportRot = poseA.orientation;

            for (int s = 1; s <= samples; ++s)
            {
                double u = static_cast<double>(s) / samples;
                JPH::RVec3 pt = EvalHermitePos(data.pA, data.tA, data.pB, data.tB, u);
                double ds = (pt - prevPt).Length();
                data.arcTable[s] = data.arcTable[s - 1] + ds;

                JPH::Vec3 currTan = (s == samples)
                    ? -poseB.orientation.RotateAxisZ()
                    : RVec3_To_Vec3(EvalHermiteTangent(data.pA, data.tA, data.pB, data.tB, u).Normalized());

                if (ds > 1e-7)
                {
                    double dAngle = std::acos(std::clamp(static_cast<double>(prevTan.Dot(currTan)), -1.0, 1.0));
                    double curvature = dAngle / ds;
                    data.maxCurvature = std::max(data.maxCurvature, curvature);
                }

                JPH::Quat stepRot = JPH::Quat::sFromTo(prevTan, currTan);
                transportRot = (stepRot * transportRot).Normalized();

                prevPt = pt;
                prevTan = currTan;
            }

            data.arcLength = data.arcTable.back();

            JPH::Vec3 transportedUp = transportRot.RotateAxisY();
            JPH::Vec3 targetUp      = poseB.orientation.RotateAxisY();
            JPH::Vec3 targetFwd     = -poseB.orientation.RotateAxisZ();

            double cosTwist = std::clamp(static_cast<double>(transportedUp.Dot(targetUp)), -1.0, 1.0);
            double sinTwist = static_cast<double>(transportedUp.Cross(targetUp).Dot(targetFwd));
            data.totalTwistAngle = static_cast<float>(std::atan2(sinTwist, cosTwist));

            return data;
        }
    }

    PipeSplinePlanner::Result PipeSplinePlanner::Plan(
        const Types::PhysPose& startPose,
        const Types::PhysPose& endPose,
        const Params& params)
    {
        Result result;
        const double D = static_cast<double>(params.pipeDiameter);

        result.isClosedLoop = ((endPose.position - startPose.position).Length() < 1e-3);

        // 1. Формирование опорных точек (кольцо, плоская S-кривая или 3D-спираль)
        std::vector<Types::PhysPose> waypoints;
        waypoints.push_back(startPose);

        if (result.isClosedLoop)
        {
            const double loopRadius   = D * 6.0;
            const double verticalWave = D * 2.0;

            JPH::RVec3 fwd0  = Vec3_To_RVec3(-startPose.orientation.RotateAxisZ());
            JPH::RVec3 side0 = Vec3_To_RVec3(startPose.orientation.RotateAxisX());
            JPH::RVec3 up0   = Vec3_To_RVec3(startPose.orientation.RotateAxisY());

            JPH::RVec3 center = startPose.position + side0 * loopRadius;

            JPH::RVec3 pos1 = center + fwd0 * loopRadius + up0 * verticalWave;
            JPH::Quat  rot1 = (JPH::Quat::sRotation(JPH::Vec3::sAxisY(), -JPH::JPH_PI * 0.5f) * startPose.orientation).Normalized();

            JPH::RVec3 pos2 = center + side0 * loopRadius;
            JPH::Quat  rot2 = (JPH::Quat::sRotation(JPH::Vec3::sAxisY(), -JPH::JPH_PI) * startPose.orientation).Normalized();

            JPH::RVec3 pos3 = center - fwd0 * loopRadius - up0 * verticalWave;
            JPH::Quat  rot3 = (JPH::Quat::sRotation(JPH::Vec3::sAxisY(), -JPH::JPH_PI * 1.5f) * startPose.orientation).Normalized();

            waypoints.push_back({pos1, rot1});
            waypoints.push_back({pos2, rot2});
            waypoints.push_back({pos3, rot3});
            waypoints.push_back(startPose);
        }
        else
        {
            SplineSpanData directTest = AnalyzeSpan(startPose, endPose, params.minBendRadius, 128);
            const double maxAllowedCurvature = 1.0 / params.minBendRadius;

            if (directTest.maxCurvature <= maxAllowedCurvature)
            {
                waypoints.push_back(endPose);
            }
            else
            {
                JPH::Vec3 f0 = -startPose.orientation.RotateAxisZ();
                JPH::Vec3 f1 = -endPose.orientation.RotateAxisZ();
                double chordLen = (endPose.position - startPose.position).Length();
                JPH::Vec3 chordDir = (chordLen > 1e-6)
                    ? RVec3_To_Vec3((endPose.position - startPose.position) / chordLen)
                    : f0;

                JPH::Vec3 outOfPlane = f0.Cross(chordDir);
                if (outOfPlane.LengthSq() < 1e-4f)
                    outOfPlane = f0.Cross(f1);
                if (outOfPlane.LengthSq() < 1e-4f)
                    outOfPlane = startPose.orientation.RotateAxisY();
                outOfPlane = outOfPlane.Normalized();

                double spiralRadius = std::max(params.minBendRadius * 1.8, chordLen * 0.6);

                JPH::RVec3 midCenter = (startPose.position + endPose.position) * 0.5;
                JPH::RVec3 dir0 = Vec3_To_RVec3(f0);
                JPH::RVec3 dir1 = Vec3_To_RVec3(f1);
                JPH::RVec3 normal3D = Vec3_To_RVec3(outOfPlane);

                JPH::RVec3 spiralApex = midCenter
                                      + normal3D * (spiralRadius * 1.6)
                                      + (dir0 - dir1) * (spiralRadius * 0.6);

                JPH::Vec3 apexTangent = (chordLen < params.minBendRadius)
                    ? (-f0 + outOfPlane.Cross(f0)).Normalized()
                    : chordDir;

                JPH::Quat midSlerp = startPose.orientation.SLERP(endPose.orientation, 0.5f).Normalized();
                JPH::Vec3 currentMidFwd = -midSlerp.RotateAxisZ();
                JPH::Quat spiralRot = (JPH::Quat::sFromTo(currentMidFwd, apexTangent) * midSlerp).Normalized();

                waypoints.push_back({spiralApex, spiralRot});
                waypoints.push_back(endPose);
            }
        }

        // 2. Анализ всей кривой и расчёт оптимальной длины сегмента
        const int cSamples = 512;
        std::vector<SplineSpanData> spans;
        double globalMaxCurvature = 0.0;
        double totalAbsTwist = 0.0;

        for (size_t w = 0; w + 1 < waypoints.size(); ++w)
        {
            SplineSpanData span = AnalyzeSpan(waypoints[w], waypoints[w + 1], params.minBendRadius, cSamples);
            result.totalLength += span.arcLength;
            globalMaxCurvature = std::max(globalMaxCurvature, span.maxCurvature);
            totalAbsTwist += std::abs(span.totalTwistAngle);
            spans.push_back(std::move(span));
        }

        const double maxTurnRad = JPH::DegreesToRadians(params.maxTurnDeg);
        double maxLenByCurvature = (globalMaxCurvature > 1e-6) ? (maxTurnRad / globalMaxCurvature) : result.totalLength;

        const double maxTwistRad = JPH::DegreesToRadians(params.maxTwistPerSegDeg);
        double maxLenByTwist = (totalAbsTwist > 1e-6) ? (result.totalLength * maxTwistRad / totalAbsTwist) : result.totalLength;

        double upperSegmentLen = std::max(D * 0.5, std::min(maxLenByCurvature, maxLenByTwist));
        int totalSegments = std::max(4, static_cast<int>(std::ceil(result.totalLength / upperSegmentLen)));
        result.optimalSegmentLength = result.totalLength / totalSegments;

        // 3. Равномерная расстановка поз по всей длине трассы
        auto samplePoseAt = [&](double s) -> Types::PhysPose
        {
            s = std::clamp(s, 0.0, result.totalLength);
            double accum = 0.0;

            for (size_t idx = 0; idx < spans.size(); ++idx)
            {
                const auto& sp = spans[idx];
                if (s <= accum + sp.arcLength || idx + 1 == spans.size())
                {
                    double localS = std::clamp(s - accum, 0.0, sp.arcLength);
                    auto it = std::lower_bound(sp.arcTable.begin(), sp.arcTable.end(), localS);
                    int k = std::clamp(static_cast<int>(std::distance(sp.arcTable.begin(), it)), 1, cSamples);

                    double l0 = sp.arcTable[k - 1];
                    double l1 = sp.arcTable[k];
                    double frac = (l1 > l0) ? (localS - l0) / (l1 - l0) : 0.0;
                    double u = (static_cast<double>(k - 1) + frac) / cSamples;

                    JPH::RVec3 pos = (u >= 0.999999) ? sp.pB : ((u <= 0.000001) ? sp.pA : EvalHermitePos(sp.pA, sp.tA, sp.pB, sp.tB, u));

                    JPH::Quat rot = sp.rotA;
                    JPH::Vec3 prevTan = -sp.rotA.RotateAxisZ();
                    int subSteps = std::max(1, static_cast<int>(u * 64.0));
                    for (int sub = 1; sub <= subSteps; ++sub)
                    {
                        double subU = u * (static_cast<double>(sub) / subSteps);
                        JPH::Vec3 curTan = (subU >= 0.999999)
                            ? -sp.rotB.RotateAxisZ()
                            : RVec3_To_Vec3(EvalHermiteTangent(sp.pA, sp.tA, sp.pB, sp.tB, subU).Normalized());
                        rot = (JPH::Quat::sFromTo(prevTan, curTan) * rot).Normalized();
                        prevTan = curTan;
                    }

                    JPH::Quat twistQuat = JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), -sp.totalTwistAngle * static_cast<float>(u));
                    JPH::Quat finalRot = (u >= 0.999999) ? sp.rotB : (rot * twistQuat).Normalized();

                    return {pos, finalRot};
                }
                accum += sp.arcLength;
            }
            return waypoints.back();
        };

        int limitIndex = result.isClosedLoop ? totalSegments : (totalSegments + 1);
        result.poses.reserve(limitIndex);
        for (int i = 0; i < limitIndex; ++i)
        {
            result.poses.push_back(samplePoseAt(i * result.optimalSegmentLength));
        }

        return result;
    }
}
