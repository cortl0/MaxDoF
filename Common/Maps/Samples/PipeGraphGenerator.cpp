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
    void PipeGraphGenerator::GenerateGraph(
        Types::LevelGraphs::PureLevelGraph& graph,
        JPH::RVec3 halfSize,
        JPH::Quat endRotation,
        const Common::LevelGraphs::Pipe::Config& config
    )
    {
        const auto& interfacesBaseIndex = graph.interfaces.size();
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
        triangle0.FillGraphVerticesAndCreateInterface(graph);
        triangle1.FillGraphVerticesAndCreateInterface(graph);
        pipe.SetTriangleInterfaceKeys({graph.interfaces[interfacesBaseIndex].id, graph.interfaces[interfacesBaseIndex + 1].id});
        pipe.FillGraph(graph, config);
    }
}
