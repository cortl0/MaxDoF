// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Utils/PipeMath.h"

#include <cmath>
#include <algorithm>
#include <random>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Math.h>

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

    float getRandomFloat(float min, float max) {
        // static гарантирует, что генератор инициализируется один раз,
        // а не при каждом вызове функции
        static std::random_device rd;
        static std::mt19937 gen(rd());
        
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    JPH::Quat getRandomEulerRotation30Deg()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        constexpr float max_rad = JPH::DegreesToRadians(30.0f);
        std::uniform_real_distribution<float> dist(-max_rad, max_rad);

        // Генерируем случайные углы для всех трёх осей (в радианах)
        float pitch = dist(gen); // Тангаж (вращение вокруг X)
        float yaw   = dist(gen); // Рысканье (вращение вокруг Y)
        float roll  = dist(gen); // Крен (вращение вокруг Z)

        // Собираем кватернион из углов Эйлера
        return JPH::Quat::sEulerAngles(JPH::Vec3(pitch, yaw, roll));
    }

    JPH::Vec3 RVec3_To_Vec3(const JPH::RVec3& p)
    {
        return
        {
            static_cast<float>(p.GetX()),
            static_cast<float>(p.GetY()),
            static_cast<float>(p.GetZ())
        };
    }

    JPH::RVec3 Vec3_To_RVec3(const JPH::Vec3& p)
    {
        return
        {
            static_cast<double>(p.GetX()),
            static_cast<double>(p.GetY()),
            static_cast<double>(p.GetZ())
        };
    }

    // 1. Локальный поворот (относительно осей самого объекта)
    // Например: повернуть самолёт вверх по его собственному тангажу
    JPH::Quat rotateLocal(const JPH::Quat& base_rot, const JPH::Quat& delta_rot)
    {
        return (base_rot * delta_rot).Normalized();
    }

    // 2. Глобальный поворот (относительно мировых осей)
    // Например: закрутить объект вокруг мировой вертикальной оси Y
    JPH::Quat rotateGlobal(const JPH::Quat& base_rot, const JPH::Quat& delta_rot)
    {
        return (delta_rot * base_rot).Normalized();
    }
}
