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
        Types3d::TetrahedronVertices vertices;
        Types3d::Interfaces interfaces;

    public:
        Tetrahedron();
        //const JPH::RVec3& pos, const JPH::Quat& ori
        const Types3d::Interfaces& GetInterfaces(){ return interfaces; }
        const Types3d::TetrahedronVertices& GetVertices(){ return vertices; }
        void SetInterfaces(const Types3d::Interfaces& interfaces){ this->interfaces = interfaces; }
        void SetVertices(const Types3d::TetrahedronVertices& vertices){ this->vertices = vertices; }
        void SetVetricesAndInterfacesBySphereRadius(double tetrahedronSphereRadius);
    };
}
