// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include <cstdint>
#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameCommon/LevelMeshes/MeshBuilder.h"

namespace MaxDoF::GameCommon::LevelMeshes
{
    void MeshBuilder::BuildMeshData(GameCommon::LevelMeshes::MeshData& meshData)
    {
        meshData =
        {
            .vertices =
            {
#ifdef MAXDOF_CLIENT_BUILD
                { -5.0f,  -5.0f, -10.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f },
                {  5.0f,  -5.0f, -10.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f },
                {  0.0f,   5.0f, -10.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f }
#else
                { -5.0f,  -5.0f, -10.0f },
                {  5.0f,  -5.0f, -10.0f },
                {  0.0f,   5.0f, -10.0f }
#endif
            },
            .indices = { 0, 1, 2 }
        };
    }
}
