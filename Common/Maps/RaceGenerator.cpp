// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/RaceGenerator.h"

#include "Common/Meshes/Constants.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Tetrahedron.h"
#include "Common/Meshes/Triangle.h"
#include "Common/Utils/PipeMath.h"
#include "Common/Utils/PipeSplinePlanner.h"

namespace MaxDoF::Common::Maps
{

    void RaceGenerator::GenerateGraph
    (
        Types::Level::Graph& graph,
        const Types::Level::Graph::Triangle::Key beginTriangleKey,
        const Types::Level::Graph::Triangle::Key endTriangleKey
    )
    {
        const float cPipeDiameter = LevelGraph::cMinTriangleEdgeLength * 10.0f;
        const float cPipeRadius = cPipeDiameter * 0.5f;
        Types::Level::Graph::Graph::Triangle::Vertices v1 = graph.GetTriangleVertices(beginTriangleKey);
        Types::Level::Graph::Graph::Triangle::Vertices v2 = graph.GetTriangleVertices(endTriangleKey);
        //std::swap(v1[1], v1[2]);
        std::swap(v2[1], v2[2]);
        const auto plan = Utils::PipeSplinePlanner::Plan(
            Utils::CalculatePoseFromVertices(v1),
            Utils::CalculatePoseFromVertices(v2),
            {
                .pipeDiameter      = cPipeDiameter,
                .maxTurnDeg        = 15.0f,
                .maxTwistPerSegDeg = 15.0f,
                .minBendRadius     = cPipeDiameter * 2.0
            }
        );

        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;
        const LevelGraphs::Pipe::Config config{};

        for (std::size_t i = 0; i < plan.poses.size(); ++i)
        {
            if (i > 0)
            {
                triangle.GenerateRegularTriangleVertices(cPipeRadius);
                triangle.Translate(plan.poses[i].position);
                triangle.RotateAroundPivot(plan.poses[i].position, plan.poses[i].orientation);
                triangle.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, false});
            }

            if (i == 1)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.begin.key = beginTriangleKey;
                cfg.end.key = graph.orientedTriangles.size() - 1;
                pipe.FillGraph(graph, cfg);
                continue;
            }
            else
            if (i == plan.poses.size() - 1)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.begin.key = graph.orientedTriangles.size() - 2;
                cfg.end.key = endTriangleKey;
                cfg.end.flip = true;
                pipe.FillGraph(graph, cfg);
            }
            else
            if (i > 0)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.begin.key = graph.orientedTriangles.size() - 2;
                cfg.end.key = graph.orientedTriangles.size() - 1;
                pipe.FillGraph(graph, cfg);
            }
        }

        if (plan.isClosedLoop && plan.poses.size() > 2)
        {
            LevelGraphs::Pipe::Config closeCfg = config;
            closeCfg.begin.key = graph.orientedTriangles.size() - 1;
            closeCfg.end.key = 0;
            pipe.FillGraph(graph, closeCfg);
        }
    }

    void RaceGenerator::GenerateGraph(Types::Level::Graph& graph)
    {
        const float cArenaRadius = 50.0f;
        const auto& tetrahedronIndex0 = graph.nodes.size();
        LevelGraphs::Tetrahedron tetrahedron;
        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;

        tetrahedron.GenerateVetrices(JPH::RVec3::sZero(), cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                .sides
                {{
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false},
                    {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false}
                }}
            }
        );

        const auto& pipeIndex0 = graph.nodes.size();

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto& tetrahedronNode = graph.nodes[tetrahedronIndex0];
            triangle.SetVertices(graph,
                graph.triangles[
                    graph.orientedTriangles[tetrahedronNode.orientedTriangleKeys[i]].triangleKey]
            );
            triangle.ScaleAroundPivot(triangle.CalculateMiddle(triangle.GetVertices()), 0.5f);
            triangle.Translate(-triangle.CalculateNormal() * cArenaRadius);
            triangle.FillGraph(graph, {Types::Level::Graph::OrientedTriangle::Type::cUnDefined, false});
            pipe.FillGraph(graph,
                {
                    .begin
                    {
                        .key = tetrahedronNode.orientedTriangleKeys[i]
                    },
                    .end
                    {
                        .key = graph.orientedTriangles.size() - 1
                    },
                    false
                }
            );
        }

        GenerateGraph(graph, 4, 5);
        GenerateGraph(graph, 6, 7);
        return;

        const auto& tetrahedronIndex1 = graph.nodes.size();
        {
            LevelGraphs::Tetrahedron tetrahedron;
            tetrahedron.GenerateVetrices(JPH::RVec3::sZero(), cArenaRadius * 6.0f);

            tetrahedron.FillGraph(graph,
                {
                    .sides
                    {{
                        {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, false},
                        {Types::Level::Graph::OrientedTriangle::Type::cSolidRock, false},
                        {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, false},
                        {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, false}
                    }}
                }
            );
        }

        for (std::size_t i = 2; i < 4; ++i)
        {
            const auto& pipeNode0 = graph.nodes[pipeIndex0 + i];
            const auto& tetrahedronNode1 = graph.nodes[tetrahedronIndex1];
            pipe.FillGraph
            (
                graph,
                {
                    .begin
                    {
                        .key = pipeNode0.orientedTriangleKeys[1]
                    },
                    .end
                    {
                        .key = tetrahedronNode1.orientedTriangleKeys[i]
                    },
                    false
                }
            );
        }
    }
}
