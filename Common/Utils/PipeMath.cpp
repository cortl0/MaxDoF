// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Utils/PipeMath.h"

namespace MaxDoF::Common::Utils
{
    Types::PhysPose CalculatePoseFromVertices(const Types::Level::Graph::Triangle::Vertices& vertices)
    {
        // 1. Позиция — центр треугольника
        JPH::RVec3 center = (vertices[0] + vertices[1] + vertices[2]) / 3.0;

        // 2. Векторы сторон в float (JPH::Vec3)
        JPH::Vec3 e1 = RVec3_To_Vec3(vertices[1] - vertices[0]);
        JPH::Vec3 e2 = RVec3_To_Vec3(vertices[2] - vertices[0]);

        // Обход против часовой стрелки (CCW): нормаль e1 × e2 направлена "к себе" (+Z),
        // соответственно направление вперёд (-Z) смотрит "от себя"
        JPH::Vec3 axisZ = e1.Cross(e2).Normalized();

        // 3. Ось +Y (направление от центра к первой вершине vertices[0] для точного восстановления крена)
        JPH::Vec3 axisY = RVec3_To_Vec3(vertices[0] - center);
        axisY = (axisY - axisZ * axisY.Dot(axisZ)).Normalized();

        // 4. Ось +X (правая тройка векторов: X = Y × Z)
        JPH::Vec3 axisX = axisY.Cross(axisZ).Normalized();

        // 5. Матрица поворота -> кватернион
        JPH::Mat44 rotMat(
            JPH::Vec4(axisX, 0.0f),
            JPH::Vec4(axisY, 0.0f),
            JPH::Vec4(axisZ, 0.0f),
            JPH::Vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        return {
            .position    = center,
            .orientation = rotMat.GetQuaternion().Normalized()
        };
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
}
