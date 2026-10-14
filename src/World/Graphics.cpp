// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "World/Graphics.h"

#include <OgreItem.h>
#include <OgreSceneManager.h>

#include "Utils/MeshProcedures.h"

namespace MaxDoF::World
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
    }

    Ogre::SceneNode* Graphics::createCube(const Types::GraphPose& pose, const Ogre::Vector3& halfSize)
    {
        auto graphPose = Types::GraphPose
        {
            .position = { 1.0f, 0.0f, -5.0f },
            .orientation = Ogre::Quaternion::IDENTITY
        };
        auto* node = createShape(ortCubeMeshName, graphPose);
        node->setScale( halfSize * 2.0f );
        return node;
    }

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

    Ogre::SceneNode* Graphics::createShape(const Ogre::String& meshName, const Types::GraphPose& pose)
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

    Ogre::SceneNode* Graphics::createSphere(const Types::GraphPose& pose, float radius)
    {
        auto* node = createShape( ortSphereMeshName, pose );
        node->setScale( Ogre::Vector3::UNIT_SCALE * radius * 2.0f );
        return node;
    }
}  // namespace MaxDoF::World
