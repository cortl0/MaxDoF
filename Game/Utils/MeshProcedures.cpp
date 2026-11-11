// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information
#include "GameClient/Utils/MeshProcedures.h"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>

#include <OgreSceneManager.h>
#include <OgreManualObject2.h>
#include <OgreMesh2.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

#include "GameCommon/Utils/SystemUtils.h"
#include "GameCommon/Types/Types3d.h"

namespace MaxDoF::GameClient::Utils
{
    bool testName(const Ogre::String &meshName)
    {
        Ogre::MeshManager *meshManager = Ogre::MeshManager::getSingletonPtr();
        return meshManager->getByName(meshName).operator bool();
    }

    void createProceduralCubeV2( Ogre::SceneManager *sceneManager, const Ogre::String &meshName, float hX, float hY, float hZ )
    {
        if (testName(meshName))
            return;

        Ogre::ManualObject* manual = sceneManager->createManualObject();
        unsigned int numVertices = 24;
        unsigned int numIndices = 36;
        manual->estimateVertexCount(numVertices);
        manual->estimateIndexCount(numIndices);
        manual->begin("CubeMaterial", Ogre::OT_TRIANGLE_LIST);

        // --- 1. ПЕРЕДНЯЯ ГРАНЬ ---
        manual->position(-hX, -hY,  hZ); manual->normal(0, 0, 1); manual->textureCoord(0.0f, 1.0f);
        manual->position( hX, -hY,  hZ); manual->normal(0, 0, 1); manual->textureCoord(1.0f, 1.0f);
        manual->position( hX,  hY,  hZ); manual->normal(0, 0, 1); manual->textureCoord(1.0f, 0.0f);
        manual->position(-hX,  hY,  hZ); manual->normal(0, 0, 1); manual->textureCoord(0.0f, 0.0f);
        manual->quad(0, 1, 2, 3);

        // --- 2. ЗАДНЯЯ ГРАНЬ ---
        manual->position( hX, -hY, -hZ); manual->normal(0, 0, -1); manual->textureCoord(0.0f, 1.0f);
        manual->position(-hX, -hY, -hZ); manual->normal(0, 0, -1); manual->textureCoord(1.0f, 1.0f);
        manual->position(-hX,  hY, -hZ); manual->normal(0, 0, -1); manual->textureCoord(1.0f, 0.0f);
        manual->position( hX,  hY, -hZ); manual->normal(0, 0, -1); manual->textureCoord(0.0f, 0.0f);
        manual->quad(4, 5, 6, 7);

        // --- 3. ЛЕВАЯ ГРАНЬ ---
        manual->position(-hX, -hY, -hZ); manual->normal(-1, 0, 0); manual->textureCoord(0.0f, 1.0f);
        manual->position(-hX, -hY,  hZ); manual->normal(-1, 0, 0); manual->textureCoord(1.0f, 1.0f);
        manual->position(-hX,  hY,  hZ); manual->normal(-1, 0, 0); manual->textureCoord(1.0f, 0.0f);
        manual->position(-hX,  hY, -hZ); manual->normal(-1, 0, 0); manual->textureCoord(0.0f, 0.0f);
        manual->quad(8, 9, 10, 11);

        // --- 4. ПРАВАЯ ГРАНЬ ---
        manual->position( hX, -hY,  hZ); manual->normal(1, 0, 0); manual->textureCoord(0.0f, 1.0f);
        manual->position( hX, -hY, -hZ); manual->normal(1, 0, 0); manual->textureCoord(1.0f, 1.0f);
        manual->position( hX,  hY, -hZ); manual->normal(1, 0, 0); manual->textureCoord(1.0f, 0.0f);
        manual->position( hX,  hY,  hZ); manual->normal(1, 0, 0); manual->textureCoord(0.0f, 0.0f);
        manual->quad(12, 13, 14, 15);

        // --- 5. ВЕРХНЯЯ ГРАНЬ ---
        manual->position(-hX,  hY,  hZ); manual->normal(0, 1, 0); manual->textureCoord(0.0f, 1.0f);
        manual->position( hX,  hY,  hZ); manual->normal(0, 1, 0); manual->textureCoord(1.0f, 1.0f);
        manual->position( hX,  hY, -hZ); manual->normal(0, 1, 0); manual->textureCoord(1.0f, 0.0f);
        manual->position(-hX,  hY, -hZ); manual->normal(0, 1, 0); manual->textureCoord(0.0f, 0.0f);
        manual->quad(16, 17, 18, 19);

        // --- 6. НИЖНЯЯ ГРАНЬ ---
        manual->position(-hX, -hY, -hZ); manual->normal(0, -1, 0); manual->textureCoord(0.0f, 1.0f);
        manual->position( hX, -hY, -hZ); manual->normal(0, -1, 0); manual->textureCoord(1.0f, 1.0f);
        manual->position( hX, -hY,  hZ); manual->normal(0, -1, 0); manual->textureCoord(1.0f, 0.0f);
        manual->position(-hX, -hY,  hZ); manual->normal(0, -1, 0); manual->textureCoord(0.0f, 0.0f);
        manual->quad(20, 21, 22, 23);

        manual->end();

        Ogre::MeshPtr mesh = manual->convertToMesh(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mesh->load();
        sceneManager->destroyManualObject(manual);
    }

    void createProceduralSphereV2(Ogre::SceneManager* sceneManager, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments)
    {
        if (testName(meshName))
            return;

        auto* manual = sceneManager->createManualObject();
        unsigned int numVertices = (rings + 1) * (segments + 1);
        unsigned int numIndices = rings * segments * 6;
        manual->estimateVertexCount(numVertices);
        manual->estimateIndexCount(numIndices);
        manual->begin("CubeMaterial", Ogre::OT_TRIANGLE_LIST);

        float fDeltaRingAngle = (Ogre::Math::PI / rings);
        float fDeltaSeqAngle = (Ogre::Math::TWO_PI / segments);

        // Генерируем вершины (ГЕОМЕТРИЯ)
        for (unsigned int ring = 0; ring <= rings; ++ring)
        {
            float r0 = radius * Ogre::Math::Sin(ring * fDeltaRingAngle);
            float y0 = radius * Ogre::Math::Cos(ring * fDeltaRingAngle);

            for (unsigned int seg = 0; seg <= segments; ++seg)
            {
                float x0 = r0 * Ogre::Math::Sin(seg * fDeltaSeqAngle);
                float z0 = r0 * Ogre::Math::Cos(seg * fDeltaSeqAngle);

                manual->position(x0, y0, z0);

                Ogre::Vector3 normal(x0, y0, z0);
                normal.normalise();
                manual->normal(normal);
                manual->textureCoord((float)seg / (float)segments, (float)ring / (float)rings);
            }
        }

        // 2. Строим индексный буфер (ТОПОЛОГИЯ)
        // В ManualObject2 вместо ->triangle() мы последовательно пушим индексы вершин
        for (unsigned int ring = 0; ring < rings; ++ring)
        {
            for (unsigned int seg = 0; seg < segments; ++seg)
                {
                unsigned int w = segments + 1;

                unsigned int v0 = ring * w + seg;
                unsigned int v1 = ring * w + (seg + 1);
                unsigned int v2 = (ring + 1) * w + seg;
                unsigned int v3 = (ring + 1) * w + (seg + 1);

                // Первый треугольник полигона (v0 -> v2 -> v1)
                manual->index(v0);
                manual->index(v2);
                manual->index(v1);

                // Второй треугольник полигона (v1 -> v2 -> v3)
                manual->index(v1);
                manual->index(v2);
                manual->index(v3);
            }
        }

        manual->end();
        Ogre::MeshPtr mesh = manual->convertToMesh(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mesh->load();
        sceneManager->destroyManualObject(manual);
    }

    void createMeshFromMeshData(Ogre::SceneManager* sceneManager, const Ogre::String& meshName, const MaxDoF::GameCommon::Types3d::MeshData& meshData)
    {
        if (Ogre::MeshManager::getSingleton().resourceExists(meshName))
        {
            return;
        }

        if (meshData.vertices.empty() || meshData.indices.empty())
        {
            MaxDoF_LOG_AND_ABORT("bad data")
        }

        Ogre::ManualObject* manual = sceneManager->createManualObject();

        // По нашему Flat-алгоритму количество вершин в ManualObject станет
        // строго равно количеству индексов, так как каждый треугольник получает свои уникальные вершины!
        manual->estimateVertexCount(meshData.indices.size());
        manual->estimateIndexCount(meshData.indices.size());
        manual->begin("CubeMaterial", Ogre::OT_TRIANGLE_LIST);

        // Счетчик добавленных графических вершин
        GameCommon::Types3d::VerticesIndexKey vertexCounter = 0;

        // Идем строго тройками индексов (обрабатываем каждый треугольник отдельно)
        for (size_t i = 0; i < meshData.indices.size(); i += 3)
        {
            // 1. Извлекаем исходные данные вершин треугольника из MeshData
            const auto& v0 = meshData.vertices[meshData.indices[i]];
            const auto& v1 = meshData.vertices[meshData.indices[i + 1]];
            const auto& v2 = meshData.vertices[meshData.indices[i + 2]];

            // 2. Строим Jolt-векторы позиций
            JPH::Vec3 p0(v0.x, v0.y, v0.z);
            JPH::Vec3 p1(v1.x, v1.y, v1.z);
            JPH::Vec3 p2(v2.x, v2.y, v2.z);

            // 3. МАТЕМАТИЧЕСКИЙ РАСЧЕТ ЧЕСТНОЙ FLAT-НОРМАЛИ ТРЕУГОЛЬНИКА (КАК В ФИЗИКЕ)
            JPH::Vec3 edge1 = p1 - p0;
            JPH::Vec3 edge2 = p2 - p0;
            JPH::Vec3 flatNormal = edge1.Cross(edge2).Normalized();

            // 4. ЗАПИСЫВАЕМ ВЕРШИНУ 0
            manual->position(v0.x, v0.y, v0.z);
            manual->normal(flatNormal.GetX(), flatNormal.GetY(), flatNormal.GetZ()); // Роботизированная нормаль
            manual->textureCoord(v0.u, v0.v);
            manual->index(vertexCounter++);

            // 5. ЗАПИСЫВАЕМ ВЕРШИНУ 1
            manual->position(v1.x, v1.y, v1.z);
            manual->normal(flatNormal.GetX(), flatNormal.GetY(), flatNormal.GetZ());
            manual->textureCoord(v1.u, v1.v);
            manual->index(vertexCounter++);

            // 6. ЗАПИСЫВАЕМ ВЕРШИНУ 2
            manual->position(v2.x, v2.y, v2.z);
            manual->normal(flatNormal.GetX(), flatNormal.GetY(), flatNormal.GetZ());
            manual->textureCoord(v2.u, v2.v);
            manual->index(vertexCounter++);
        }

        manual->end();
        Ogre::MeshPtr mesh = manual->convertToMesh(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mesh->load();
        sceneManager->destroyManualObject(manual);
    }

    void removeMesh(const Ogre::String& meshName)
    {
        Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();

        if (meshManager.resourceExists(meshName))
        {
            Ogre::ResourcePtr meshResource = meshManager.getByName(
                meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );

            if (!meshResource.operator bool())
            {
                meshResource->unload();

                meshManager.remove(meshResource);
            }
        }
    }

}
