// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/Types3d.h"
#include "Common/Types/LevelGraphStructures.h"

 namespace MaxDoF::Common::LevelGraphs
{
    class Triangle
    {
    private:
        Types3d::TriangleVertices vertices;

    public:
        const Types3d::TriangleVertices& GetVertices() const noexcept { return vertices; }
        void SetVertices(const Types3d::TriangleVertices& vertices){ this->vertices = vertices; }
        void SetVertices(const Types::LevelGraphs::PureLevelGraph&, const Types::LevelGraphs::TriangleInterface&);
        JPH::RVec3 CalculateMiddle() const;
        JPH::RVec3 CalculateMiddle(const Types3d::TriangleVertices&) const;
        JPH::RVec3 CalculateNormal() const;
        JPH::RVec3 CalculateNormal(const Types3d::TriangleVertices&) const;
        JPH::Quat CalculateRotationQuat(JPH::RVec3 axis, float degrees);
        void FillGraphVertices(Types::LevelGraphs::PureLevelGraph &) const;
        void FillGraphVerticesAndCreateInterface(Types::LevelGraphs::PureLevelGraph&) const;
        void GenerateRegularTriangleVertices(float radius);
        void Rotate(const JPH::Quat& rotation);
        void RotateAroundPivot(const JPH::RVec3& pivot, const JPH::Quat& rotation);
        void Scale(double factor);
        void ScaleAroundPivot(const JPH::RVec3 &pivot, double factor);
        void Translate(JPH::RVec3 offset);
    };
}
