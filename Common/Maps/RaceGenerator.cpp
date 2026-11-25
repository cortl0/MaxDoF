// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/RaceGenerator.h"

#include "Common/Meshes/Constants.h"
#include "Common/Meshes/Triangle.h"
#include "Common/Utils/PipeMath.h"

namespace MaxDoF::Common::Maps
{
    void RaceGenerator::GenerateGraph(
        Types::Level::Graph& graph,
        JPH::RVec3 halfSize,
        JPH::Quat endRotation,
        const LevelGraphs::Pipe::Config& config
    )
    {
        // const auto& interfacesBaseIndex = graph.triangles.size();
        LevelGraphs::Triangle triangle;
        const float cPipeRadius = LevelGraph::cMinTriangleEdgeLength * 2.0;
        const float cPipeLength = 10.0f;
        const Types::PhysPose p0{{0.0, 0.0, -cPipeLength}, JPH::Quat::sIdentity()};
        Types::PhysPose p1{p0};
        Types::PhysPose p2{p1};
        Types::PhysPose p3{p1};

        LevelGraphs::Pipe pipe;

        for(int i = 0; i < 100; ++i)
        {
            triangle.GenerateRegularTriangleVertices(cPipeRadius);
            triangle.Translate(p1.position);
            triangle.RotateAroundPivot(p1.position, p1.orientation);
            triangle.FillGraphVerticesAndCreateInterface(graph, {Types::Level::Graph::OrientedTriangle::Type::SolidRock, false});

            if(i > 1)
            {
                pipe.SetTriangleKeys({graph.orientedTriangles.size() - 2, graph.orientedTriangles.size() - 1});
                pipe.FillGraph(graph, {{true, false}, true,
                    {
                        Types::Level::Graph::OrientedTriangle::Type::SolidRock,
                        Types::Level::Graph::OrientedTriangle::Type::SolidRock
                    }}
                );
            }

            p2 = p1;
            p3 = p1;
            p2.orientation = Utils::rotateLocal(p2.orientation, Utils::getRandomEulerRotation30Deg());
            p3.orientation = Utils::rotateLocal(p3.orientation, Utils::getRandomEulerRotation30Deg());
            p2.position = p1.position + Utils::Vec3_To_RVec3(p2.orientation * Utils::RVec3_To_Vec3(JPH::RVec3{0.0, 0.0, -cPipeLength}));
            p3.position = p1.position + Utils::Vec3_To_RVec3(p3.orientation * Utils::RVec3_To_Vec3(JPH::RVec3{0.0, 0.0, -cPipeLength}));
            if(Utils::ChooseOptimalPose(p1, p2, p3) == 2)
                p1 = p2;
            else
                p1 = p3;
        }
    }
}
