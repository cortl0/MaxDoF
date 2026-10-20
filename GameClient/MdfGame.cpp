// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/MdfGame.h"

#include <iostream>

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
        std::cout << "<<<<<<<<<<<<<<<<<<<<camera->getNearClipDistance()" << camera->getNearClipDistance() << std::endl; //0.2
        std::cout << "<<<<<<<<<<<<<<<<<<<<camera->setFarClipDistance()" << camera->getFarClipDistance() << std::endl; //1000
        // myCamera->setNearClipDistance(0.05f);
        camera->setFarClipDistance(10000.0f);
        if( camera->getParentSceneNode() )
            camera->getParentSceneNode()->detachObject( camera );
        auto& shipEntity = mShip->getEntity();
        shipEntity.graphNode->attachObject( camera );
        camera->setPosition( Ogre::Vector3::ZERO );
        //camera->setPosition( Ogre::Vector3( 0.0f, 0.0f, 10.0f ) );
        camera->setOrientation( Ogre::Quaternion::IDENTITY );

        mShipController = std::make_unique<ShipController>(
            shipEntity.graphNode,
            mWorldManager.getBodyInterface(),
            &shipEntity.physId
        );
    }

    void MdfGame::createScene01()
    {
        mScene = std::make_unique<GameCommon::Scenes::Scene01>(mWorldManager);
        mWorldManager.initialize( mMaxBodiesLimit );
        mScene->createScene();
        auto& shipEntity = mScene->createShipEntity();

#ifdef MAXDOF_CLIENT_BUILD
        mShip = new GameClient::Ship::Ship(mGraphicsSystem->getSceneManager(), shipEntity);
        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
#endif
        connectCameraAndControllerToShip();

        mGraphicsSystem->getSceneManager()->setForwardClustered(
            true,     // bEnable: Активируем систему
            4,       // width: Разрешение сетки по горизонтали. 16 — стандарт для 1080p/4K.
            4,        // height: Разрешение по вертикали.
            24,       // numSlices: 24 слоя глубины дают плавное распределение света без резких переключений.
            8,        // lightsPerCell: Максимум 8 источников на одну ячейку экрана. Для кабины (3 прибора + пара ламп) этого хватит с запасом.
            1,        // decalsPerCell: Минимальное безопасное значение 1 (для экономии памяти, раз декали не используем).
            1,        // cubemapProbesPerCel: Минимальное безопасное значение 1.
            0.05f,    // minDistance: КРИТИЧЕСКИ ВАЖНО. Ставим 5 см от камеры, чтобы свет приборов не гас, когда камера пилота приближается к Dashboard.
            500.0f    // maxDistance: Дальность работы точечного света. 500 единиц хватит, чтобы видеть свет от взрывов или пролетающих рядом ракет.
        );
    }

    void MdfGame::update( float timeSinceLast )
    {
        // 1. Шагаем физику (Jolt считает мир и автоматически двигает Ogre-узлы из mEntities)
        mWorldManager.update( timeSinceLast, mShip->getEntity().physId );
        //mShip->mEntity.graphNode->setPosition(Ogre::Vector3::ZERO);
        // 2. Опрашиваем устройства ввода через правильные методы транслятора
        auto data = mInputTranslator.translate(&mKeyboardController, &mMouseController);

        // 3. Передаем полностью собранные данные в контроллер корабля
        if( mShipController.operator bool() )
            mShipController->update( timeSinceLast, data );
                mMouseController.reset();

#ifdef MAXDOF_CLIENT_BUILD
        if( mTextOverleyManager.mDisplayHelpMode != 0 )
        {
            auto telemetry = mShipController->getTelemetry(timeSinceLast);
            mTextOverleyManager.generateDebugText( timeSinceLast, telemetry);
            mShip->update(timeSinceLast, telemetry);
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
            const float timeSinceLast = 0.0f;
            auto telemetry = mShipController->getTelemetry(timeSinceLast);
            mTextOverleyManager.generateDebugText( timeSinceLast, telemetry);
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
}
