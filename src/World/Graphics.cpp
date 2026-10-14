// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "World/Graphics.h"

#include "OgreItem.h"
#include "OgreSceneManager.h"

#include "OgreManualObject2.h"

#include "OgreMesh2.h"
#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"

#include "OgreCamera.h"
#include "OgreWindow.h"

#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsSamplerblock.h"

#include "OgreRoot.h"

#include "OgreGpuProgramManager.h"
#include "OgreHlms.h"
#include "OgreHlmsCompute.h"
#include "OgreHlmsManager.h"

#include "Utils/Converters.h"
#include "Utils/MeshProcedures.h"
#include "Utils/Types.h"

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

    Ogre::SceneNode* Graphics::createCube()
    {
        auto graphPose = Types::GraphPose
        {
            .position = { 1.0f, 0.0f, -5.0f },
            .orientation = Ogre::Quaternion::IDENTITY
        };
        return createGraphShape(ortCubeMeshName, graphPose);
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

    Ogre::SceneNode* Graphics::createGraphShape(const Ogre::String& meshName, const Types::GraphPose& pose)
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

    Ogre::SceneNode* Graphics::createSphere()
    {
        auto graphPose = Types::GraphPose
        {
            .position = { -1.0f, 0.0f, -5.0f },
            .orientation = Ogre::Quaternion::IDENTITY
        };
        return createGraphShape(ortSphereMeshName, graphPose);
    }
}  // namespace MaxDoF::World
