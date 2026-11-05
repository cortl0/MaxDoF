// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Pipe.h"

#include "GameCommon/LevelMeshes/MeshBuilder.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    Pipe::Pipe()
    {
    }

    void Pipe::SetVetricesAndInterfaces(JPH::RVec3 posBegin, JPH::RVec3 posEnd, double radius)
    {
        const float cFixedRotation = 3.1415926535f / 6.0f;

        // Стабильный базис плоскости сечения трубы (X-вправо, Y-вверх)
        JPH::RVec3 U(1.0f, 0.0f, 0.0f);
        JPH::RVec3 V(0.0f, 1.0f, 0.0f);

        vertices = {{
           GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posBegin, U, V, 0.0f, radius),   // Вершина 2 (Вперед-влево-вниз)
           GameCommon::LevelMeshes::MeshBuilder::GenerateTriangleVertices(posEnd, U, V, cFixedRotation, radius)     // Вершина 3 (Вперед-вправо-вниз)
        }};

        // interfaces = {{
        //     {0, 2, 1}, // Грань 0 Левая
        //     {0, 1, 3}, // Грань 1 Правая
        //     {0, 3, 2}, // Грань 2 Нижняя
        //     {1, 2, 3}  // Грань 3 Переднияя
        // }};
    }
}
