// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/RaceGenerator.h"

#include "Common/Meshes/Constants.h"
#include "Common/Meshes/Triangle.h"
#include "Common/Utils/PipeMath.h"
#include "Common/Utils/PipeSplinePlanner.h"

namespace MaxDoF::Common::Maps
{
    void RaceGenerator::GenerateGraph(
        Types::Level::Graph& graph,
        JPH::RVec3 halfSize,
        JPH::Quat endRotation,
        const LevelGraphs::Pipe::Config& config
    )
    {
        const float cPipeDiameter = LevelGraph::cMinTriangleEdgeLength * 5.0f;
        const float cPipeRadius   = cPipeDiameter * 0.5f;

        const Types::PhysPose startPose{{50.0, 50.0, 50.0}, JPH::Quat::sIdentity()};
        const Types::PhysPose endPose{halfSize, endRotation};

        const auto plan = Utils::PipeSplinePlanner::Plan(
            startPose,
            endPose,
            {
                .pipeDiameter      = cPipeDiameter,
                .maxTurnDeg        = 30.0f,
                .maxTwistPerSegDeg = 10.0f
            }
        );

        LevelGraphs::Triangle triangle;
        LevelGraphs::Pipe pipe;

        for (size_t i = 0; i < plan.poses.size(); ++i)
        {
            triangle.GenerateRegularTriangleVertices(cPipeRadius);
            triangle.Translate(plan.poses[i].position);
            triangle.RotateAroundPivot(plan.poses[i].position, plan.poses[i].orientation);
            triangle.FillGraphVerticesAndCreateInterface(graph, {config.orientedTriangleTypes[0], false});

            if (i > 0)
            {
                LevelGraphs::Pipe::Config cfg = config;
                cfg.triangleKeys =
                {
                    graph.orientedTriangles.size() - 2,
                    graph.orientedTriangles.size() - 1
                };
                pipe.FillGraph(graph, cfg);
            }
        }

        if (plan.isClosedLoop && plan.poses.size() > 2)
        {
            LevelGraphs::Pipe::Config closeCfg = config;
            closeCfg.triangleKeys =
            {
                graph.orientedTriangles.size() - 1,
                0
            };
            pipe.FillGraph(graph, closeCfg);
        }
    }
}
