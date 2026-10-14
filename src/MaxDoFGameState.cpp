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
        mTextOverleyManager( helpDescription )
    {
    }

    MaxDoFGameState::~MaxDoFGameState()
    {
    }

    void MaxDoFGameState::_notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
        mWorldManager.getGraphics()._notifyGraphicsSystem( mGraphicsSystem );
        mTextOverleyManager._notifyGraphicsSystem(graphicsSystem);
    }

    void MaxDoFGameState::createCube()
    {
        Types::PhysPose pose
        {
            .position = JPH::RVec3( 1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mWorldManager.createCube(pose, Types::cUnitScaleVector * 0.5f, 1000.0f);
    }

    void MaxDoFGameState::createLight()
    {
        mWorldManager.createLight();
    }

    void MaxDoFGameState::createSphere()
    {
        Types::PhysPose pose
        {
            .position = JPH::RVec3( -1.0f, 0.0f, -5.0f ),
            .orientation = Types::cIdentity
        };
        mWorldManager.createSphere(pose, 0.5f, 1000.0f);
    }

    void MaxDoFGameState::createShip()
    {
        mShip = mWorldManager.createSphere(Types::cDefaultPhysPose, 0.5f, 5000.0f);

        Ogre::Camera *camera = mGraphicsSystem->getCamera();
        if( camera->getParentSceneNode() )
            camera->getParentSceneNode()->detachObject( camera );
        mShip.graphNode->attachObject( camera );
        camera->setPosition( Ogre::Vector3::ZERO );
        // camera->setPosition( Ogre::Vector3( 0.0f, 0.0f, 10.0f ) );
        camera->setOrientation( Ogre::Quaternion::IDENTITY );

        mSpacecraftController = std::make_unique<SpacecraftController>(
            mGraphicsSystem,
            mShip.graphNode,
            mWorldManager.getBodyInterface(),
            &mShip.physId
        );
    }

    void MaxDoFGameState::createScene01()
    {
        mWorldManager.initialize( mMaxBodiesLimit );

        createLight();
        createShip();
        createCube();
        createSphere();

        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
    }

    void MaxDoFGameState::update( float timeSinceLast )
    {
        // 1. Шагаем физику (Jolt считает мир и автоматически двигает Ogre-узлы из mEntities)
        mWorldManager.update( timeSinceLast );

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

    void MaxDoFGameState::keyPressed( const SDL_KeyboardEvent &arg )
    {
        bool handledEvent = false;
        handledEvent = mKeyboardController.keyPressed( arg );
        if( !handledEvent )
            GameState::keyPressed( arg );
    }

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

    void MaxDoFGameState::mouseMoved( const SDL_Event &arg )
    {
        mMouseController.mouseMoved( arg );
        GameState::mouseMoved( arg );
    }
}  // namespace MaxDoF
