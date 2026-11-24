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
        const Common::LevelGraphs::Pipe::Config& config
    )
    {
        const auto& interfacesBaseIndex = graph.triangles.size();
        LevelGraphs::Triangle triangle0;
        LevelGraphs::Pipe pipe;
        const float cPipeRadius = LevelGraph::cMinTriangleEdgeLength * 2.0;
        const float cPipeLength = 50.0f;
        JPH::RVec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::RVec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);
        triangle0.GenerateRegularTriangleVertices(cPipeRadius);
        //triangle0.Translate(pBegin + halfSize);
        triangle0.FillGraphVerticesAndCreateInterface(graph);
        


        pipe.SetTriangleInterfaceKeys({graph.triangles[interfacesBaseIndex].id, graph.triangles[interfacesBaseIndex + 1].id});
        pipe.FillGraph(graph, config);
    }
}
