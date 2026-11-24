// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "AcousticWorld.h"

namespace MaxDoF::Game::Audio
{
    AcousticWorld::AcousticWorld()
    {
        IPLContextSettings ctxSettings{};
        ctxSettings.version = STEAMAUDIO_VERSION;
        iplContextCreate(&ctxSettings, &mIPLContext);

        IPLAudioSettings audioSettings{ cAudioSampleRate, cAudioFrameSize };
        IPLHRTFSettings hrtfSettings{};
        hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;
        hrtfSettings.volume = 1.0f;
        iplHRTFCreate(mIPLContext, &audioSettings, &hrtfSettings, &mIPLHRTF);

        IPLSceneSettings sceneSettings{};
        sceneSettings.type = IPL_SCENETYPE_DEFAULT;
        iplSceneCreate(mIPLContext, &sceneSettings, &mIPLScene);

        IPLSimulationSettings simSettings{};
        simSettings.flags = IPL_SIMULATIONFLAGS_DIRECT;
        simSettings.sceneType = IPL_SCENETYPE_DEFAULT;
        simSettings.maxNumOcclusionSamples = 16;
        iplSimulatorCreate(mIPLContext, &simSettings, &mIPLSimulator);
        iplSimulatorSetScene(mIPLSimulator, mIPLScene);
    }

    AcousticWorld::~AcousticWorld()
    {
        ReleaseAllMeshes();
        if (mIPLSimulator) iplSimulatorRelease(&mIPLSimulator);
        if (mIPLScene) iplSceneRelease(&mIPLScene);
        if (mIPLHRTF) iplHRTFRelease(&mIPLHRTF);
        if (mIPLContext) iplContextRelease(&mIPLContext);
    }

    int AcousticWorld::AddDynamicMesh(const Vertices& vertices,
                                    const Triangles& triangles,
                                    const IPLMaterial& material,
                                    const IPLMatrix4x4& initialTransform)
    {
        DynamicMesh dyn{};

        // 1. Создаем отдельную локальную сцену для этого объекта
        IPLSceneSettings subSceneSettings{};
        subSceneSettings.type = IPL_SCENETYPE_DEFAULT;
        iplSceneCreate(mIPLContext, &subSceneSettings, &dyn.subScene);

        // 2. Кладем меш в локальную сцену
        IPLStaticMeshSettings meshSettings{};
        meshSettings.numVertices = static_cast<IPLint32>(vertices.size());
        meshSettings.numTriangles = static_cast<IPLint32>(triangles.size());
        meshSettings.numMaterials = 1;
        meshSettings.vertices = const_cast<IPLVector3*>(vertices.data());
        meshSettings.triangles = const_cast<IPLTriangle*>(triangles.data());
        std::vector<IPLint32> matIndices(triangles.size(), 0);
        meshSettings.materialIndices = matIndices.data();
        meshSettings.materials = const_cast<IPLMaterial*>(&material);

        iplStaticMeshCreate(dyn.subScene, &meshSettings, &dyn.staticMesh);
        iplStaticMeshAdd(dyn.staticMesh, dyn.subScene);
        iplSceneCommit(dyn.subScene); // Строим локальное дерево ОДИН раз!

        // 3. Создаем инстанс в основной сцене с матрицей трансформации
        IPLInstancedMeshSettings instSettings{};
        instSettings.subScene = dyn.subScene;
        instSettings.transform = initialTransform;

        iplInstancedMeshCreate(mIPLScene, &instSettings, &dyn.instancedMesh);
        iplInstancedMeshAdd(dyn.instancedMesh, mIPLScene);

        mDynamicMeshes.push_back(dyn);
        return static_cast<int>(mDynamicMeshes.size() - 1);
    }

    void AcousticWorld::AddStaticMesh(
        const Vertices& vertices,
        const Triangles& triangles,
        const IPLMaterial& material
    )
    {
        IPLStaticMeshSettings meshSettings{};
        meshSettings.numVertices = static_cast<IPLint32>(vertices.size());
        meshSettings.numTriangles = static_cast<IPLint32>(triangles.size());
        meshSettings.numMaterials = 1;
        meshSettings.vertices = const_cast<IPLVector3*>(vertices.data());
        meshSettings.triangles = const_cast<IPLTriangle*>(triangles.data());

        std::vector<IPLint32> matIndices(triangles.size(), 0);
        meshSettings.materialIndices = matIndices.data();
        meshSettings.materials = const_cast<IPLMaterial*>(&material);

        IPLStaticMesh mesh = nullptr;
        if (iplStaticMeshCreate(mIPLScene, &meshSettings, &mesh) == IPL_STATUS_SUCCESS)
        {
            iplStaticMeshAdd(mesh, mIPLScene);
            mIPLStaticMeshes.push_back(mesh);
        }
    }

    void AcousticWorld::ClearScene()
    {
        ReleaseAllMeshes();

        if (mIPLScene)
            iplSceneCommit(mIPLScene);

        if (mIPLSimulator)
            iplSimulatorCommit(mIPLSimulator);
    }

    void AcousticWorld::CommitScene()
    {
        iplSceneCommit(mIPLScene);
        iplSimulatorCommit(mIPLSimulator);
    }

    IPLMatrix4x4 AcousticWorld::MakeIPLTransform(
        float px, float py, float pz,
        float qx, float qy, float qz, float qw,
        float sx, float sy, float sz
    ) const
    {
        // Предварительные произведения компонент кватерниона
        float xx = qx * qx;
        float yy = qy * qy;
        float zz = qz * qz;
        float xy = qx * qy;
        float xz = qx * qz;
        float yz = qy * qz;
        float wx = qw * qx;
        float wy = qw * qy;
        float wz = qw * qz;

        IPLMatrix4x4 m{};
        float* e = &m.elements[0][0]; // Плоский указатель на 16 элементов (Row-Major)

        // Строка 0 (ось X с учетом масштаба sx)
        *(e + 0)  = (1.0f - 2.0f * (yy + zz)) * sx;
        *(e + 1)  = (2.0f * (xy - wz))        * sy;
        *(e + 2)  = (2.0f * (xz + wy))        * sz;
        *(e + 3)  = px; // Перенос по X

        // Строка 1 (ось Y с учетом масштаба sy)
        *(e + 4)  = (2.0f * (xy + wz))        * sx;
        *(e + 5)  = (1.0f - 2.0f * (xx + zz)) * sy;
        *(e + 6)  = (2.0f * (yz - wx))        * sz;
        *(e + 7)  = py; // Перенос по Y

        // Строка 2 (ось Z с учетом масштаба sz)
        *(e + 8)  = (2.0f * (xz - wy))        * sx;
        *(e + 9)  = (2.0f * (yz + wx))        * sy;
        *(e + 10) = (1.0f - 2.0f * (xx + yy)) * sz;
        *(e + 11) = pz; // Перенос по Z

        // Строка 3 (однородные координаты)
        *(e + 12) = 0.0f;
        *(e + 13) = 0.0f;
        *(e + 14) = 0.0f;
        *(e + 15) = 1.0f;

        return m;
    }

    void AcousticWorld::ReleaseAllMeshes()
    {
        for (auto& mesh : mIPLStaticMeshes)
            if (mesh)
            {
                iplStaticMeshRemove(mesh, mIPLScene);
                iplStaticMeshRelease(&mesh);
            }

        mIPLStaticMeshes.clear();

        for (auto& dyn : mDynamicMeshes)
        {
            if (dyn.instancedMesh)
            {
                iplInstancedMeshRemove(dyn.instancedMesh, mIPLScene);
                iplInstancedMeshRelease(&dyn.instancedMesh);
            }

            if (dyn.staticMesh)
            {
                if (dyn.subScene)
                    iplStaticMeshRemove(dyn.staticMesh, dyn.subScene);

                iplStaticMeshRelease(&dyn.staticMesh);
            }

            if (dyn.subScene)
                iplSceneRelease(&dyn.subScene);
        }
        mDynamicMeshes.clear();
    }

    void AcousticWorld::RunDirectSimulation(const IPLCoordinateSpace3& listener)
    {
        IPLSimulationSharedInputs sharedInputs{};
        sharedInputs.listener = listener;
        iplSimulatorSetSharedInputs(mIPLSimulator, IPL_SIMULATIONFLAGS_DIRECT, &sharedInputs);
        iplSimulatorRunDirect(mIPLSimulator);
    }

    void AcousticWorld::UpdateDynamicMeshTransform(int meshId, const IPLMatrix4x4& transform)
    {
        if (meshId < 0 || meshId >= static_cast<int>(mDynamicMeshes.size())) return;
        iplInstancedMeshUpdateTransform(mDynamicMeshes.at(meshId).instancedMesh, mIPLScene, transform);
    }
}
