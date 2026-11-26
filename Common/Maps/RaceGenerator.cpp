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
                .maxTwistPerSegDeg = 15.0f
            }
        );

        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;
        const LevelGraphs::Pipe::Config config
        {
            false,
            .begin
            {
                .type = Types::Level::Graph::OrientedTriangle::Type::cOpenSpace,
                .flip = true
            },
            .end
            {
                .type = Types::Level::Graph::OrientedTriangle::Type::cOpenSpace,
                .flip = false
            }
        };

        for (std::size_t i = 0; i < plan.poses.size(); ++i)
        {
            if (i > 0)
            {
                triangle.GenerateRegularTriangleVertices(cPipeRadius);
                triangle.Translate(plan.poses[i].position);
                triangle.RotateAroundPivot(plan.poses[i].position, plan.poses[i].orientation);
                triangle.FillGraphVerticesAndCreateInterface(graph, {config.begin.type, false});
            }

            if (i == 1)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.begin.key = graph.GetOrientedTriangleKeys(0)[beginTriangleKey];
                cfg.end.key = graph.orientedTriangles.size() - 1;
                pipe.FillGraph(graph, cfg);
                continue;
            }
            else
            if (i == plan.poses.size() - 1)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.begin.key = graph.orientedTriangles.size() - 2;
                cfg.end.key = graph.GetOrientedTriangleKeys(0)[endTriangleKey];
                cfg.end.flipAntiprismSide = true;
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
        LevelGraphs::Tetrahedron tetrahedron;
        tetrahedron.GenerateVetrices(JPH::RVec3::sZero(), cArenaRadius);
        tetrahedron.FillGraph(graph,
            {
                .sides
                {{
                    {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, true},
                    {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, true},
                    {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, true},
                    {Types::Level::Graph::OrientedTriangle::Type::cOpenSpace, true}
                }}
            }
        );

        GenerateGraph(graph, 0, 1);
        GenerateGraph(graph, 2, 3);
    }
}
