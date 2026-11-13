// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Maps/Samples/PipeGraphGenerator.h"

#include "Common/Meshes/Constants.h"
#include "Common/Meshes/MeshBuilder.h"
#include "Common/Meshes/Pipe.h"
#include "Common/Meshes/Triangle.h"

namespace MaxDoF::Common::Maps
{
    void PipeGraphGenerator::GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset, std::array<bool, 2> invertInterfaces, bool invertPipe, JPH::Quat rotation)
    {
        const auto& interfacesBaseIndex = graph.interfaces.size();
        Common::LevelGraphs::Triangle triangle0, triangle1;
        Common::LevelGraphs::Pipe pipe;
        const float cPipeRadius = Common::LevelGraph::cMinTriangleEdgeLength * 2.0;
        const float cPipeLength = 50.0f;
        JPH::RVec3 pBegin(0.0f, 0.0f, -cPipeLength / 2.0f);
        JPH::RVec3 pEnd(0.0f, 0.0f, cPipeLength / 2.0f);
        triangle0.GenerateRegularTriangleVertices(cPipeRadius);
        triangle1.SetVertices(triangle0.GetVertices());
        triangle0.Translate(pBegin + offset);
        triangle1.RotateAroundPivot(pBegin, rotation);
        triangle1.Translate(pEnd + offset);
        triangle0.FillGraphVerticesAndCreateInterface(graph);
        triangle1.FillGraphVerticesAndCreateInterface(graph);
        pipe.SetTriangleInterfaceKeys({
            graph.interfaces[interfacesBaseIndex].id,
            graph.interfaces[interfacesBaseIndex + 1].id
        });
        pipe.FillGraph(graph, invertInterfaces, invertPipe, {Types::LevelGraphs::InterfaceType::SolidRock, Types::LevelGraphs::InterfaceType::SolidRock});
    }
}
