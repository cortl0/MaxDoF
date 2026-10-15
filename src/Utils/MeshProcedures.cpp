// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "Utils/MeshProcedures.h"

#include <OgreSceneManager.h>
#include <OgreManualObject2.h>
#include <OgreMesh2.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

namespace MaxDoF::Utils
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
        manual->begin("BaseWhite", Ogre::OT_TRIANGLE_LIST); 

        // --- 1. ПЕРЕДНЯЯ ГРАНЬ (Нормаль смотрит строго на вас: 0, 0, 1) ---
        manual->position(-hX, -hY,  hZ); manual->normal(0, 0, 1);
        manual->position( hX, -hY,  hZ); manual->normal(0, 0, 1);
        manual->position( hX,  hY,  hZ); manual->normal(0, 0, 1);
        manual->position(-hX,  hY,  hZ); manual->normal(0, 0, 1);
        manual->quad(0, 1, 2, 3);

        // --- 2. ЗАДНЯЯ ГРАНЬ (Нормаль смотрит вглубь экрана: 0, 0, -1) ---
        manual->position( hX, -hY, -hZ); manual->normal(0, 0, -1);
        manual->position(-hX, -hY, -hZ); manual->normal(0, 0, -1);
        manual->position(-hX,  hY, -hZ); manual->normal(0, 0, -1);
        manual->position( hX,  hY, -hZ); manual->normal(0, 0, -1);
        manual->quad(4, 5, 6, 7);

        // --- 3. ЛЕВАЯ ГРАНЬ (Нормаль смотрит влево: -1, 0, 0) ---
        manual->position(-hX, -hY, -hZ); manual->normal(-1, 0, 0);
        manual->position(-hX, -hY,  hZ); manual->normal(-1, 0, 0);
        manual->position(-hX,  hY,  hZ); manual->normal(-1, 0, 0);
        manual->position(-hX,  hY, -hZ); manual->normal(-1, 0, 0);
        manual->quad(8, 9, 10, 11);

        // --- 4. ПРАВАЯ ГРАНЬ (Нормаль смотрит вправо: 1, 0, 0) ---
        manual->position( hX, -hY,  hZ); manual->normal(1, 0, 0);
        manual->position( hX, -hY, -hZ); manual->normal(1, 0, 0);
        manual->position( hX,  hY, -hZ); manual->normal(1, 0, 0);
        manual->position( hX,  hY,  hZ); manual->normal(1, 0, 0);
        manual->quad(12, 13, 14, 15);

        // --- 5. ВЕРХНЯЯ ГРАНЬ (Нормаль смотрит вверх: 0, 1, 0) ---
        manual->position(-hX,  hY,  hZ); manual->normal(0, 1, 0);
        manual->position( hX,  hY,  hZ); manual->normal(0, 1, 0);
        manual->position( hX,  hY, -hZ); manual->normal(0, 1, 0);
        manual->position(-hX,  hY, -hZ); manual->normal(0, 1, 0);
        manual->quad(16, 17, 18, 19);

        // --- 6. НИЖНЯЯ ГРАНЬ (Нормаль смотрит вниз: 0, -1, 0) ---
        manual->position(-hX, -hY, -hZ); manual->normal(0, -1, 0);
        manual->position( hX, -hY, -hZ); manual->normal(0, -1, 0);
        manual->position( hX, -hY,  hZ); manual->normal(0, -1, 0);
        manual->position(-hX, -hY,  hZ); manual->normal(0, -1, 0);
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
        manual->begin("BaseWhite", Ogre::OT_TRIANGLE_LIST);

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
}  // namespace MaxDoF::Utils
