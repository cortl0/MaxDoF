// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "GameCommon/Types/Types3d.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    class Tetrahedron
    {
        Types3d::Tetrahedron::Vertices vertices;
        Types3d::Tetrahedron::TrianglesVerticesIndices interfaces;

    public:
        Tetrahedron();
        const Types3d::Tetrahedron::TrianglesVerticesIndices& GetInterfaces(){ return interfaces; }
        const Types3d::Tetrahedron::Vertices& GetVertices(){ return vertices; }
        void SetInterfaces(const Types3d::Tetrahedron::TrianglesVerticesIndices& interfaces){ this->interfaces = interfaces; }
        void SetVertices(const Types3d::Tetrahedron::Vertices& vertices){ this->vertices = vertices; }
        void SetVetricesAndInterfaces(double tetrahedronSphereRadius);
    };
}
