// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/Types3d.h"
#include "Common/Types/LevelGraphStructures.h"
#include "Primitive.h"

 namespace MaxDoF::Common::LevelGraphs
{
    class Pipe : public Primitive
    {
    public:
        using Vertices = std::array<Types3d::TriangleVertices, 2>;
        using TrianglesVerticesIndices = std::array<Types3d::TriangleVerticesIndices, 2>;
        using TriangleInterfaceKeys = std::array<Types::LevelGraphs::TriangleInterfaceKey, 2>;

    private:
        TriangleInterfaceKeys mTriangleInterfaceKeys;
        TrianglesVerticesIndices interfaces
        {{
            {0, 1, 2}, // Торец 0 Передний
            {3, 4, 5}  // Торец 1 Задний
        }};

    public:
        // const TrianglesVerticesIndices& GetInterfaces() const noexcept { return interfaces; }
        // const Vertices& GetVertices() const noexcept { return vertices; }
        // void SetInterfaces(const TrianglesVerticesIndices& interfaces){ this->interfaces = interfaces; }
        void SetTriangleInterfaceKeys(const TriangleInterfaceKeys& Keys){ mTriangleInterfaceKeys = Keys; }
        // void SetVertices(const Vertices& vertices){ this->vertices = vertices; }
        void FillGraph(Types::LevelGraphs::PureLevelGraph &, std::array<bool, 2> invertInterfaces, bool invertPipe, std::array<Types::LevelGraphs::InterfaceType, 2> interfaceTypes);
    };
}
