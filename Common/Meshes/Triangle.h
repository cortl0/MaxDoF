// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraph.h"

 namespace MaxDoF::Common::LevelGraphs
{
    class Triangle
    {
    private:
        Types::Level::Graph::Triangle::Vertices vertices;

    public:
        struct Config
        {
            Types::Level::Graph::OrientedTriangle::Type type;
            bool inverted;
        };

        const Types::Level::Graph::Triangle::Vertices& GetVertices() const noexcept { return vertices; }
        void SetVertices(const Types::Level::Graph::Triangle::Vertices& vertices){ this->vertices = vertices; }
        void SetVertices(const Types::Level::Graph&, const Types::Level::Graph::Triangle&);
        JPH::RVec3 CalculateMiddle() const;
        JPH::RVec3 CalculateMiddle(const Types::Level::Graph::Triangle::Vertices&) const;
        JPH::RVec3 CalculateNormal() const;
        JPH::RVec3 CalculateNormal(const Types::Level::Graph::Triangle::Vertices&) const;
        JPH::Quat CalculateRotationQuat(JPH::RVec3 axis, float degrees);
        void FillGraphVertices(Types::Level::Graph &) const;
        void FillGraphVerticesAndCreateInterface(Types::Level::Graph&, const Config&) const;
        void GenerateRegularTriangleVertices(float radius);
        void Rotate(const JPH::Quat& rotation);
        void RotateAroundPivot(const JPH::RVec3& pivot, const JPH::Quat& rotation);
        void Scale(double factor);
        void ScaleAroundPivot(const JPH::RVec3 &pivot, double factor);
        void Translate(JPH::RVec3 offset);
    };
}
