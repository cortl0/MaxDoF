// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/TriangleGraphGenerator.h"

#include "Common/Meshes/Triangle.h"
#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void TriangleGraphGenerator::GenerateGraph(Types::Level::Graph& graph, const JPH::RVec3& offset)
    {
        LevelGraphs::Triangle triangle;
        const float cRadius = 1.0f;
        triangle.GenerateRegularTriangleVertices(cRadius);
        triangle.Scale(25.0);
        triangle.Translate(offset);
        triangle.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, true});
        triangle.Translate({0.0, 0.0, -10.0});
        triangle.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, false});
    }
}
