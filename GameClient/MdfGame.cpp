// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/MdfGame.h"

#include <OgreCamera.h>
#include <OgreGpuProgramManager.h>
#include <OgreHlmsCompute.h>
#include <OgreHlmsManager.h>
#include <OgreRoot.h>

namespace MaxDoF::GameClient
{
    MdfGame::MdfGame( const Ogre::String &helpDescription )
        : mTextOverleyManager( helpDescription )
    {
    }

    MdfGame::~MdfGame()
    {
    }

#ifdef MAXDOF_CLIENT_BUILD
    void MdfGame::_notifyGraphicsSystem( Demo::GraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
        mWorldManager.getGraphics()._notifyGraphicsSystem( mGraphicsSystem );
        mTextOverleyManager._notifyGraphicsSystem(graphicsSystem);
    }
#endif

    void MdfGame::connectCameraAndControllerToShip()
    {
        Ogre::Camera *camera = mGraphicsSystem->getCamera();
        if( camera->getParentSceneNode() )
            camera->getParentSceneNode()->detachObject( camera );
        mShip.graphNode->attachObject( camera );
        camera->setPosition( Ogre::Vector3::ZERO );
        //camera->setPosition( Ogre::Vector3( 0.0f, 0.0f, 10.0f ) );
        camera->setOrientation( Ogre::Quaternion::IDENTITY );

        mSpacecraftController = std::make_unique<ShipController>(
            mShip.graphNode,
            mWorldManager.getBodyInterface(),
            &mShip.physId
        );
    }

    void MdfGame::createScene01()
    {
        mScene = std::make_unique<Scenes::Scene01>(mWorldManager);
        mWorldManager.initialize( mMaxBodiesLimit );
        mScene->createScene();
        mShip = mScene->createShip();
        connectCameraAndControllerToShip();

#ifdef MAXDOF_CLIENT_BUILD
        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
#endif
    }

    void MdfGame::update( float timeSinceLast )
    {
        // 1. Шагаем физику (Jolt считает мир и автоматически двигает Ogre-узлы из mEntities)
        mWorldManager.update( timeSinceLast );

        // 2. Опрашиваем устройства ввода через правильные методы транслятора
        auto data = mInputTranslator.translate(&mKeyboardController, &mMouseController);

        // 3. Передаем полностью собранные данные в контроллер корабля
        if( mSpacecraftController.operator bool() )
            mSpacecraftController->update( timeSinceLast, data );
                mMouseController.reset();

#ifdef MAXDOF_CLIENT_BUILD
        if( mTextOverleyManager.mDisplayHelpMode != 0 )
        {
            mTextOverleyManager.generateDebugText( timeSinceLast );
        }
#endif
    }

    void MdfGame::keyPressed( const SDL_KeyboardEvent &arg )
    {
        bool handledEvent = false;
        handledEvent = mKeyboardController.keyPressed( arg );
        if( !handledEvent )
            GameState::keyPressed( arg );
    }

    void MdfGame::keyReleased( const SDL_KeyboardEvent &arg )
    {
        if( arg.keysym.scancode == SDL_SCANCODE_F1 &&
            ( arg.keysym.mod & ~( KMOD_NUM | KMOD_CAPS ) ) == 0 )
        {
#ifdef MAXDOF_CLIENT_BUILD
            mTextOverleyManager.mDisplayHelpMode = ( mTextOverleyManager.mDisplayHelpMode + 1 ) % mTextOverleyManager.mNumDisplayHelpModes;

            mTextOverleyManager.generateDebugText( 0 );
#endif
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
            handledEvent = mKeyboardController.keyReleased( arg );
            if( !handledEvent )
                GameState::keyReleased( arg );
        }
    }

    void MdfGame::mouseMoved( const SDL_Event &arg )
    {
        mMouseController.mouseMoved( arg );
        GameState::mouseMoved( arg );
    }
}  // namespace MaxDoF::Client
