// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/PipeGraphGenerator.h"

#include "Common/Meshes/Constants.h"
#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void PipeGraphGenerator::GenerateGraph(Types::Level::Graph& graph)
    {
        const double pipePos = 15.0;
        const double pipePosZ = 75.0;

        Maps::PipeGraphGenerator::GenerateGraph(graph, {pipePos, pipePos, -pipePosZ}, JPH::Quat{0.0f, 0.0f, 0.25881905f, 0.96592583f},
            false, true,
        {
            .begin
            {
                .key = graph.orientedTriangles.size()
            },
            .end
            {
                .key = graph.orientedTriangles.size() + 1
            },
            true
        });

        Maps::PipeGraphGenerator::GenerateGraph(graph, {-pipePos, pipePos, -pipePosZ}, JPH::Quat{0.0f, 0.0f, -0.25881905f, 0.96592583f},
            false, true,
            {
                .begin
                {
                    .key = graph.orientedTriangles.size()
                },
                .end
                {
                    .key = graph.orientedTriangles.size() + 1
                },
                true
            }
        );

        Maps::PipeGraphGenerator::GenerateGraph(graph, {pipePos, -pipePos, -pipePosZ}, JPH::Quat{0.0f, 0.0f, 0.25881905f, 0.96592583f},
            true, false,
            {
                .begin
                {
                    .key = graph.orientedTriangles.size()
                },
                .end
                {
                    .key = graph.orientedTriangles.size() + 1
                },
                false
            }
        );

        Maps::PipeGraphGenerator::GenerateGraph(graph, {-pipePos, -pipePos, -pipePosZ}, JPH::Quat{0.0f, 0.0f, -0.25881905f, 0.96592583f},
            true, false,
            {
                .begin
                {
                    .key = graph.orientedTriangles.size()
                },
                .end
                {
                    .key = graph.orientedTriangles.size() + 1
                },
                false
            }
        );
    }

    void PipeGraphGenerator::GenerateGraph(
        Types::Level::Graph& graph,
        JPH::RVec3 halfSize,
        JPH::Quat endRotation,
        bool beginFlip,
        bool endFlip,
        const LevelGraphs::Pipe::Config& config
    )
    {
        LevelGraphs::Triangle triangle0, triangle1;
        LevelGraphs::Pipe pipe;
        const float cPipeRadius = LevelGraph::cMinTriangleEdgeLength * 2.0;
        const float cPipeLength = 50.0f;
        JPH::RVec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::RVec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);
        triangle0.GenerateRegularTriangleVertices(cPipeRadius);
        triangle1.SetVertices(triangle0.GetVertices());
        triangle0.Translate(pBegin + halfSize);
        triangle1.RotateAroundPivot(pBegin, endRotation);
        triangle1.Translate(pEnd + halfSize);
        triangle0.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, beginFlip});
        triangle1.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, endFlip});
        pipe.FillGraph(graph, config);
    }

}
