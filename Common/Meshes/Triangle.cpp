// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Meshes/Triangle.h"

#include "Common/Meshes/MeshBuilder.h"

namespace MaxDoF::Common::LevelGraphs
{
    void Triangle::SetVertices(const Types::LevelGraphs::PureLevelGraph& graph, const Types::LevelGraphs::TriangleInterface& triangleInterface)
    {
        for(std::size_t i = 0; i < 3; ++i)
            vertices[i] = graph.vertices[triangleInterface.verticesIndices[i]];
    }

    JPH::RVec3 Triangle::CalculateMiddle() const
    {
        return CalculateMiddle(vertices);
    }

    JPH::RVec3 Triangle::CalculateMiddle(const Types3d::TriangleVertices& v) const
    {
        JPH::RVec3 result = v[0];

        for(std::size_t i = 1; i < 3; ++i)
            result += v[i];

        return result / 3.0;
    }

    JPH::RVec3 Triangle::CalculateNormal() const
    {
        return CalculateNormal(vertices);
    }

    JPH::RVec3 Triangle::CalculateNormal(const Types3d::TriangleVertices& v) const
    {
        return (v[1] - v[0]).Cross((v[2] - v[0])).Normalized();
    }

    JPH::Quat Triangle::CalculateRotationQuat(JPH::RVec3 axis, float degrees)
    {
        // 1. Защита от нулевого вектора (если ось не задана, возвращаем единичный кватернион без поворота)
        float length_sq = axis.LengthSq();
        if (length_sq < 1e-8f)
            return JPH::Quat::sIdentity();

        // 2. Нормализуем вектор оси (длина должна быть строго 1)
        JPH::Vec3 normalized_axis = JPH::Vec3{
            static_cast<float>(axis.GetX()),
            static_cast<float>(axis.GetY()),
            static_cast<float>(axis.GetZ())}
            / std::sqrt(length_sq);

        // 3. Переводим градусы в радианы и создаем кватернион
        float radians = JPH::DegreesToRadians(degrees);
        return JPH::Quat::sRotation(normalized_axis, radians);
    }

    void Triangle::FillGraphVertices(Types::LevelGraphs::PureLevelGraph& graph) const
    {
        for(const auto vertex : vertices)
            graph.vertices.push_back(vertex);
    }

    void Triangle::FillGraphVerticesAndCreateInterface(Types::LevelGraphs::PureLevelGraph& graph) const
    {
        const auto baseVertexIndex = graph.vertices.size();
        FillGraphVertices(graph);

        graph.interfaces.push_back(Types::LevelGraphs::TriangleInterface
            {
                .id = graph.interfaces.size(),
                .verticesIndices = {baseVertexIndex, baseVertexIndex + 1, baseVertexIndex + 2}
            }
        );
    }

    void Triangle::GenerateRegularTriangleVertices(float radius)
    {
        const JPH::RVec3 U{1.0f, 0.0f, 0.0f};
        const JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        for (int i = 0; i < 3; ++i)
        {
            float angle = (120.0f / 180.0f * JPH::JPH_PI * static_cast<float>(i));
            float x2d = radius * std::cos(angle);
            float y2d = radius * std::sin(angle);
            vertices[i] = x2d * U + y2d * V;
        }
    }

    void Triangle::Rotate(const JPH::Quat& rotation)
    {
        for(auto& vertex : vertices)
            vertex = JPH::RVec3(rotation * JPH::Vec3(vertex));
    }

    void Triangle::RotateAroundPivot(const JPH::RVec3& pivot, const JPH::Quat& rotation)
    {
        Translate(-pivot);
        Rotate(rotation);
        Translate(pivot);
    }

    void Triangle::Scale(double factor)
    {
        for(auto& vertex : vertices)
            vertex *= factor;
    }

    void Triangle::ScaleAroundPivot(const JPH::RVec3& pivot, double factor)
    {
        Translate(-pivot);
        Scale(factor);
        Translate(pivot);
    }

    void Triangle::Translate(JPH::RVec3 offset)
    {
        for(std::size_t i = 0; i < 3; ++i)
            vertices[i] += offset;
    }
}
