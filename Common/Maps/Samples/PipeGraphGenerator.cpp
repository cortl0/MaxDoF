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
        Types::Level::Graph& graph,
        JPH::RVec3 halfSize,
        JPH::Quat endRotation,
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
        triangle0.FillGraphVerticesAndCreateInterface(graph, {config.interfaceTypes[0], config.invertInterfaces[0]});
        triangle1.FillGraphVerticesAndCreateInterface(graph, {config.interfaceTypes[1], config.invertInterfaces[1]});
        pipe.SetTriangleKeys(
            {
                graph.orientedTriangles[graph.orientedTriangles.size() - 2].triangleId,
                graph.orientedTriangles[graph.orientedTriangles.size() - 1].triangleId
            }
        );
        pipe.FillGraph(graph, config);
    }
}
