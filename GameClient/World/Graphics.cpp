// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/World/Graphics.h"

#include <iostream>
#include <OgreHlmsPbs.h>
#include <OgreHlmsPbsDatablock.h>
#include <OgreItem.h>
#include <OgreSceneManager.h>
#include <OgreTextureGpuManager.h>

#include "GameClient/Utils/MeshProcedures.h"

namespace MaxDoF::GameClient::World
{
    Graphics::Graphics( const Ogre::String &helpDescription ) :
        mTextOverleyManager(helpDescription),
        mGraphicsSystem( 0 )
    {
    }

    Graphics::~Graphics()
    {
    }

    void Graphics::_notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
    }

    void Graphics::changeTexture(Ogre::Item* cubeItem, const Ogre::String& newTextureFileName)
    {
        // 1. Получаем текущий материал (datablock) куба
        Ogre::HlmsDatablock* baseDatablock = cubeItem->getSubItem(0)->getDatablock();

        // 2. Приводим его к типу PBS (так как встроенный куб использует реалистичный PBS-материал)
        if (baseDatablock->getCreator()->getType() == Ogre::HLMS_PBS)
        {
            Ogre::HlmsPbsDatablock* pbsDatablock = static_cast<Ogre::HlmsPbsDatablock*>(baseDatablock);

            // 3. Загружаем новую текстуру через менеджер текстур Ogre-Next 4.0
            Ogre::TextureGpuManager* textureGpuManager = 
                Ogre::Root::getSingleton().getRenderSystem()->getTextureGpuManager();

            Ogre::TextureGpu* newTexture = textureGpuManager->createOrRetrieveTexture(
                newTextureFileName,
                Ogre::GpuPageOutStrategy::Discard,
                Ogre::TextureFlags::AutomaticBatching, // Флаг для обычных статических картинок
                Ogre::TextureTypes::Type2D,
                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME
            );

            // Переводим текстуру в активное состояние (загружаем в видеопамять)
            newTexture->scheduleTransitionTo(Ogre::GpuResidency::Resident);

            // 4. Меняем диффузную карту (основной цвет/рисунок) на новую текстуру
            pbsDatablock->setTexture(Ogre::PBSM_DIFFUSE, newTexture);
        }
    }

    void Graphics::changeUniqueTexture(Ogre::Item* cubeItem, const Ogre::String& newTextureFileName, const Ogre::String& uniqueMaterialName)
    {
        Ogre::Root &root = Ogre::Root::getSingleton();
        Ogre::HlmsManager *hlmsManager = root.getHlmsManager();
        Ogre::TextureGpuManager *textureGpuManager = root.getRenderSystem()->getTextureGpuManager();

        // 1. Получаем менеджер материалов HLMS PBS
        Ogre::HlmsPbs *hlmsPbs = static_cast<Ogre::HlmsPbs*>(hlmsManager->getHlms(Ogre::HLMS_PBS));

        // 2. Проверяем, не создавали ли мы этот уникальный материал ранее (чтобы избежать дублирования)
        Ogre::HlmsPbsDatablock* uniqueDatablock = static_cast<Ogre::HlmsPbsDatablock*>(
            hlmsPbs->getDatablock(uniqueMaterialName)
        );

        if (!uniqueDatablock)
        {
            // Создаем новый пустой PBS материал с вашим уникальным именем
            uniqueDatablock = static_cast<Ogre::HlmsPbsDatablock*>(
                hlmsPbs->createDatablock(
                    uniqueMaterialName, uniqueMaterialName,
                    Ogre::HlmsMacroblock(), Ogre::HlmsBlendblock(), Ogre::HlmsParamVec()
                )
            );

            // Опционально: настраиваем базовые свойства PBS (чтобы куб красиво отражал свет)
            uniqueDatablock->setRoughness(0.5f);                 // Гладкость/Блеск
            uniqueDatablock->setSpecular(Ogre::Vector3(0.1f));    // Интенсивность бликов
        }

        // 3. Загружаем вашу новую текстуру из папки ресурсов (например, ../Data)
        Ogre::TextureGpu *texture = textureGpuManager->createOrRetrieveTexture(
            newTextureFileName,
            Ogre::GpuPageOutStrategy::Discard,
            Ogre::TextureFlags::AutomaticBatching,
            Ogre::TextureTypes::Type2D,
            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME
        );
        texture->scheduleTransitionTo(Ogre::GpuResidency::Resident);

        // 4. Привязываем текстуру к новому материалу
        uniqueDatablock->setTexture(Ogre::PBSM_DIFFUSE, texture);

        // 5. Применяем этот новый уникальный материал ТОЛЬКО к этому конкретному кубу
        cubeItem->setDatablock(uniqueDatablock);
    }

    void Graphics::initialize()
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        Utils::createProceduralSphereV2( sceneManager, ortSphereMeshName, 0.5f, 16, 16 );
        // Utils::createProceduralCubeV2( sceneManager, ortCubeMeshName, 0.5f, 0.5f, 0.5f );
        printAllMaterials();

        // Ogre::Item* item = sceneManager->createItem("cube_d.mesh");
        // // Ogre::Item* item = sceneManager->createItem("Sphere1000.mesh");
        // // 2. Меняем ему текстуру на любую другую, которая есть в ресурсах (например, "rockwall.tga")
        // changeTexture(item, "rockwall.tga");

        changeUniqueTexture(sceneManager->createItem("Sphere1000.mesh"), "1d_SPIRAL.png", "MyWoodMaterial");
        changeUniqueTexture(sceneManager->createItem("cube_d.mesh"), "BeachStones.jpg", "MyStoneMaterial");
        globalStaticNode = sceneManager->getRootSceneNode()->createChildSceneNode(Ogre::SceneMemoryMgrTypes::SCENE_DYNAMIC);
    }

    Ogre::SceneNode* Graphics::createCube(const GameCommon::Types::GraphPose& pose, const Ogre::Vector3& halfSize)
    {
        auto* node = createShape("cube_d.mesh", pose);
        node->setScale( halfSize * 2.0f / 2 );
        return node;
    }

#ifdef MAXDOF_CLIENT_BUILD
    Ogre::SceneNode* Graphics::createPointLight(Ogre::Vector3 position)
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        auto* node = globalStaticNode->createChildSceneNode(Ogre::SceneMemoryMgrTypes::SCENE_DYNAMIC, position);
        auto* light = sceneManager->createLight();
        node->attachObject( light );
        light->setType( Ogre::Light::LT_POINT );
        light->setCastShadows(true);
        light->setAttenuation(100.0f, 1.0f, 0.0f, 0.01f);
        light->setShadowFarDistance(100.0f);
        return node;
    }
#endif

    Ogre::SceneNode* Graphics::createShape(const Ogre::String& meshName, const GameCommon::Types::GraphPose& pose)
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        auto* node = sceneManager->getRootSceneNode()->createChildSceneNode( Ogre::SCENE_DYNAMIC );
        Ogre::Item *item = sceneManager->createItem(
            meshName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::SCENE_DYNAMIC
        );
        item->setDatablock("MyStoneMaterial");
        node->setPosition( pose.position );
        node->setOrientation( pose.orientation );
        node->attachObject( item );
        item->setCastShadows(true);
        return node;
    }

    Ogre::SceneNode* Graphics::createSphere(const GameCommon::Types::GraphPose& pose, float radius)
    {
        auto* node = createShape( ortSphereMeshName, pose );
        node->setScale( Ogre::Vector3::UNIT_SCALE * radius * 2.0f );
        return node;
    }

    void Graphics::setPositionOfGlobalStaticNode(const Ogre::Vector3 & position)
    {
        globalStaticNode->setPosition(position);
    }

void Graphics::printAllMaterials()
{
    auto* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();

    std::cout << "=== Список зарегистрированных материалов (HLMS) ===" << std::endl;
    for (size_t i = 0; i < Ogre::HLMS_MAX; ++i)
    {
        Ogre::Hlms* hlms = hlmsManager->getHlms(static_cast<Ogre::HlmsTypes>(i));
        if (!hlms) 
            continue;

        std::string typeName = (i == Ogre::HLMS_PBS) ? "PBS" : 
                            (i == Ogre::HLMS_UNLIT) ? "Unlit" : "Other";

        std::cout << "--- Тип HLMS: " << typeName << " ---" << std::endl;

        const Ogre::Hlms::HlmsDatablockMap& datablocks = hlms->getDatablockMap();

        for (auto const& [id, entry] : datablocks)
        {
            // Извлекаем указатель на датаблок из структуры DatablockEntry
            Ogre::HlmsDatablock* datablock = entry.datablock;
            
            if (datablock)
            {
                // Получаем читаемое имя материала
                Ogre::String nameStr = datablock->getName().getFriendlyText();
                std::cout << "  Material Name: " << nameStr << std::endl;
            }
        }
    }
    std::cout << "=================================================" << std::endl;
}

}
