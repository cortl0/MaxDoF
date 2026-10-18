// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/World/Graphics.h"

#include <iostream>
#include "OgreHlmsPbs.h"
#include <OgreItem.h>
#include <OgreSceneManager.h>

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

    void Graphics::initialize()
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        Utils::createProceduralSphereV2( sceneManager, ortSphereMeshName, 0.5f, 16, 16 );
        Utils::createProceduralCubeV2( sceneManager, ortCubeMeshName, 0.5f, 0.5f, 0.5f );
        printAllMaterials();
    }

    Ogre::SceneNode* Graphics::createCube(const GameCommon::Types::GraphPose& pose, const Ogre::Vector3& halfSize)
    {
        auto graphPose = GameCommon::Types::GraphPose
        {
            .position = { 1.0f, 0.0f, -5.0f },
            .orientation = Ogre::Quaternion::IDENTITY
        };
        auto* node = createShape(ortCubeMeshName, graphPose);
        node->setScale( halfSize * 2.0f );
        return node;
    }

#ifdef MAXDOF_CLIENT_BUILD
    Ogre::SceneNode* Graphics::createLight()
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        auto* node = sceneManager->getRootSceneNode()->createChildSceneNode();
        auto* light = sceneManager->createLight();
        node->attachObject( light );
        light->setType( Ogre::Light::LT_DIRECTIONAL );
        light->setDirection( Ogre::Vector3( -1.0f, -1.0f, -1.0f ).normalisedCopy() );
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
        node->setPosition( pose.position );
        node->setOrientation( pose.orientation );
        node->attachObject( item );
        return node;
    }

    Ogre::SceneNode* Graphics::createSphere(const GameCommon::Types::GraphPose& pose, float radius)
    {
        auto* node = createShape( ortSphereMeshName, pose );
        node->setScale( Ogre::Vector3::UNIT_SCALE * radius * 2.0f );
        return node;
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

}  // namespace MaxDoF::Client::World
