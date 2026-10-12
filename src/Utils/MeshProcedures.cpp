// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "OgreSceneManager.h"

#include "OgreManualObject2.h"

#include "OgreMesh2.h"

namespace MaxDoF::Utils
{
    void createProceduralSphereV2(Ogre::SceneManager* sceneManager, const Ogre::String& meshName, float radius, unsigned int rings, unsigned int segments)
    {
        Ogre::ManualObject* manual = sceneManager->createManualObject(Ogre::SCENE_DYNAMIC);

        // 1. Обязательно резервируем место в видеопамяти (Estimate)
        // Количество вершин = (rings + 1) * (segments + 1)
        // Количество индексов для треугольников = rings * segments * 6
        unsigned int numVertices = (rings + 1) * (segments + 1);
        unsigned int numIndices = rings * segments * 6;
        manual->estimateVertexCount(numVertices);
        manual->estimateIndexCount(numIndices);

        // Начинаем геометрию
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

        manual->end(); // Теперь индексы определены, и метод выполнится успешно!

        // Превращаем в Mesh и принудительно активируем в памяти
        Ogre::MeshPtr mesh = manual->convertToMesh(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mesh->load();

        sceneManager->destroyManualObject(manual);
    }
}  // namespace MaxDoF::Utils
