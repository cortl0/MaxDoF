// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>

#include "AudioTypes.h"

namespace MaxDoF::Game::Audio
{
    struct DynamicMesh
    {
        IPLScene subScene = nullptr;
        IPLStaticMesh staticMesh = nullptr;
        IPLInstancedMesh instancedMesh = nullptr;
    };

    class AcousticWorld
    {
    public:
        AcousticWorld();
        ~AcousticWorld();

        AcousticWorld(const AcousticWorld&) = delete;
        AcousticWorld& operator=(const AcousticWorld&) = delete;

        int AddDynamicMesh(const std::vector<IPLVector3>& vertices,
                        const std::vector<IPLTriangle>& triangles,
                        const IPLMaterial& material,
                        const IPLMatrix4x4& initialTransform);

        void AddStaticMesh(const std::vector<IPLVector3>& vertices,
                        const std::vector<IPLTriangle>& triangles,
                        const IPLMaterial& material);

        void ClearScene();
        void CommitScene();
        IPLContext GetContext() const { return mIPLContext; }
        IPLHRTF GetHRTF() const { return mIPLHRTF; }
        IPLSimulator GetSimulator() const { return mIPLSimulator; }

        IPLMatrix4x4 MakeIPLTransform(
            float px, float py, float pz,
            float qx, float qy, float qz, float qw,
            float sx = 1.0f, float sy = 1.0f, float sz = 1.0f
        ) const;

        void RunDirectSimulation(const IPLCoordinateSpace3& listener);
        void UpdateDynamicMeshTransform(int meshId, const IPLMatrix4x4& transform);

    private:
        void ReleaseAllMeshes();

        IPLContext mIPLContext = nullptr;
        IPLHRTF mIPLHRTF = nullptr;
        IPLScene mIPLScene = nullptr;
        IPLSimulator mIPLSimulator = nullptr;
        std::vector<IPLStaticMesh> mIPLStaticMeshes;
        std::vector<DynamicMesh> DynamicMeshes;
    };
}