// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "MaxDoFGameState.h"

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

namespace MaxDoF
{
    MaxDoFGameState::MaxDoFGameState( const Ogre::String &helpDescription ) :
        mTextOverleyManager(helpDescription),
        mGraphicsSystem( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    MaxDoFGameState::~MaxDoFGameState()
    {
    }
    //-----------------------------------------------------------------------------------
    void MaxDoFGameState::_notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
        mTextOverleyManager._notifyGraphicsSystem(graphicsSystem);
    }

    void MaxDoFGameState::createCube()
    {
        auto* node = createGraphicShape(ortCubeMeshName);
        node->setPosition( Ogre::Vector3( 1.0f, 0.0f, -5.0f ) );
        // node->setOrientation( Ogre::Quaternion::IDENTITY );
        // node->setScale(Ogre::Vector3::UNIT_SCALE);
        Types::Pose pose
        {
            .position = JPH::RVec3( 1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mPhysicsManager.createPhysBox( node, pose, Types::cUnitScaleVector / 2.0f, 1000.0f );
    }

    void MaxDoFGameState::createLight()
    {
        Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
        Ogre::SceneNode *lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        Ogre::Light *light = sceneManager->createLight();
        lightNode->attachObject( light );
        light->setType( Ogre::Light::LT_DIRECTIONAL );
        light->setDirection( Ogre::Vector3( -1.0f, -1.0f, -1.0f ).normalisedCopy() );
    }

    Ogre::SceneNode* MaxDoFGameState::createGraphicShape(const Ogre::String& meshName)
    {
        auto* sceneManager = mGraphicsSystem->getSceneManager();
        auto* node = sceneManager->getRootSceneNode()->createChildSceneNode( Ogre::SCENE_DYNAMIC );
        Ogre::Item *item = sceneManager->createItem(
            meshName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::SCENE_DYNAMIC
        );
        node->attachObject( item );
        return node;
    }

    void MaxDoFGameState::createSphere()
    {
        auto* node = createGraphicShape(ortSphereMeshName);
        node->setPosition( Ogre::Vector3( -1.0f, 0.0f, -5.0f ) );
        Types::Pose pose
        {
            .position = JPH::RVec3( -1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mPhysicsManager.createPhysSphere( node, pose, 0.5f, 1000.0f );
    }

    void MaxDoFGameState::createShip()
    {
        auto* shipNode = createGraphicShape(ortSphereMeshName);

        Ogre::Camera *camera = mGraphicsSystem->getCamera();
        if( camera->getParentSceneNode() )
            camera->getParentSceneNode()->detachObject( camera );
        shipNode->attachObject( camera );
        camera->setPosition( Ogre::Vector3::ZERO );
        // camera->setPosition( Ogre::Vector3( 0.0f, 0.0f, 10.0f ) );
        camera->setOrientation( Ogre::Quaternion::IDENTITY );

        mShip = mPhysicsManager.createPhysSphere( shipNode, Types::cDefaultPose, 1.0f, 2000.0f );
        mSpacecraftController = std::make_unique<SpacecraftController>(
            mGraphicsSystem,
            shipNode,
            mPhysicsManager.getBodyInterface(),
            &mShip.physId
        );
    }

    void MaxDoFGameState::createScene01()
    {
        Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
        mPhysicsManager.initialize( mMaxBodiesLimit );
        Utils::createProceduralSphereV2(sceneManager, ortSphereMeshName, 0.5f, 16, 16);
        Utils::createProceduralCubeV2( sceneManager, ortCubeMeshName, 0.5f, 0.5f, 0.5f );

        createLight();
        createShip();
        createCube();
        createSphere();

        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
    }
    //-----------------------------------------------------------------------------------
    void MaxDoFGameState::update( float timeSinceLast )
    {
        // 1. Шагаем физику (Jolt считает мир и автоматически двигает Ogre-узлы из mEntities)
        mPhysicsManager.update( timeSinceLast );

        // 2. Опрашиваем устройства ввода через правильные методы транслятора
        auto data = mInputTranslator.translate(&mKeyboardController, &mMouseController);

        // 3. Передаем полностью собранные данные в контроллер корабля
        if( mSpacecraftController.operator bool() )
            mSpacecraftController->update( timeSinceLast, data );
                mMouseController.reset();

        if( mTextOverleyManager.mDisplayHelpMode != 0 )
        {
            mTextOverleyManager.generateDebugText( timeSinceLast );
        }
    }
    //-----------------------------------------------------------------------------------
    void MaxDoFGameState::keyPressed( const SDL_KeyboardEvent &arg )
    {
        bool handledEvent = false;
        handledEvent = mKeyboardController.keyPressed( arg );
        if( !handledEvent )
            GameState::keyPressed( arg );
    }
    //-----------------------------------------------------------------------------------
    void MaxDoFGameState::keyReleased( const SDL_KeyboardEvent &arg )
    {
        if( arg.keysym.scancode == SDL_SCANCODE_F1 &&
            ( arg.keysym.mod & ~( KMOD_NUM | KMOD_CAPS ) ) == 0 )
        {
            mTextOverleyManager.mDisplayHelpMode = ( mTextOverleyManager.mDisplayHelpMode + 1 ) % mTextOverleyManager.mNumDisplayHelpModes;

            mTextOverleyManager.generateDebugText( 0 );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F1 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            // Hot reload of PBS shaders. We need to clear the microcode cache
            // to prevent using old compiled versions.
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_PBS );
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F2 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            // Hot reload of Unlit shaders.
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_UNLIT );
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F3 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            // Hot reload of Compute shaders.
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getComputeHlms();
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F5 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            // Force device reelection
            Ogre::Root *root = mGraphicsSystem->getRoot();
            root->getRenderSystem()->validateDevice( true );
        }
        else
        {
            bool handledEvent = false;
            handledEvent = mKeyboardController.keyReleased( arg ); // <-- Опрашиваем наш контроллер
            if( !handledEvent )
                GameState::keyReleased( arg );
        }
    }
    //-----------------------------------------------------------------------------------
    void MaxDoFGameState::mouseMoved( const SDL_Event &arg )
    {
        mMouseController.mouseMoved( arg );
        GameState::mouseMoved( arg );
    }
}  // namespace MaxDoF
