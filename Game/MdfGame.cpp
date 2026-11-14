// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Game/MdfGame.h"

#include <iostream>

#include <OgreGpuProgramManager.h>
#include <OgreHlmsCompute.h>
#include <OgreHlmsManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>

namespace MaxDoF::Game
{
    MdfGame::MdfGame( const Ogre::String &helpDescription )
        : mTextOverleyManager( helpDescription )
    {
    }

    MdfGame::~MdfGame()
    {
    }

#ifdef MAXDOF_CLIENT_BUILD
    void MdfGame::_notifyGraphicsSystem( Core::MaxDoFGraphicsSystem *graphicsSystem )
    {
        mGraphicsSystem = graphicsSystem;
        mWorldManager.getGraphics()._notifyGraphicsSystem( mGraphicsSystem );
        mTextOverleyManager._notifyRoot( mGraphicsSystem->getRoot() );
    }
#endif

    void MdfGame::connectControllerToShip()
    {
        mShipController = std::make_unique<ShipController>( mScene->getShip(), mWorldManager.getBodyInterface() );
    }

    void MdfGame::createScene01()
    {
        mScene = std::make_unique<Common::Scenes::Scene01>( mWorldManager );
        mWorldManager.initialize( mMaxBodiesLimit );
        mWorldManager.getPhysicsSystem()->SetContactListener( &mBattleLogic.GetMyContactListener() );
        mScene->createScene();
#ifdef MAXDOF_CLIENT_BUILD
        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
#endif
        mScene->getShip().connectCameraToShip( mGraphicsSystem->getCamera() );
        connectControllerToShip();

        mGraphicsSystem->getSceneManager()->setForwardClustered(
            true,
            4,
            4,
            24,
            8,
            1,
            1,
            0.05f,
            500.0f
        );
    }

    void MdfGame::update( float timeSinceLast )
    {
        auto data = mInputTranslator.translate( &mKeyboardController, &mMouseController );
        mMouseController.reset();

        if( mShipController )
            mShipController->update( timeSinceLast, data );

        mWorldManager.update( timeSinceLast, mScene->getShip().getEntity().physId );
        mBattleLogic.Update();

#ifdef MAXDOF_CLIENT_BUILD
        if( mTextOverleyManager.mDisplayHelpMode != 0 )
        {
            auto telemetry = mShipController->getTelemetry( timeSinceLast );
            mTextOverleyManager.generateDebugText( timeSinceLast, telemetry );
            mScene->getShip().update( timeSinceLast, telemetry );
        }
#endif
    }

    void MdfGame::handleEvent( const SDL_Event &evt )
    {
        switch( evt.type )
        {
            case SDL_KEYDOWN:
                if( evt.key.repeat == 0 )
                    keyPressed( evt.key );
                break;

            case SDL_KEYUP:
                keyReleased( evt.key );
                break;

            case SDL_MOUSEMOTION:
                mouseMoved( evt );
                break;

            default:
                break;
        }
    }

    void MdfGame::keyPressed( const SDL_KeyboardEvent &arg )
    {
        mKeyboardController.keyPressed( arg );
    }

    void MdfGame::keyReleased( const SDL_KeyboardEvent &arg )
    {
        if( arg.keysym.scancode == SDL_SCANCODE_F1 &&
            ( arg.keysym.mod & ~( KMOD_NUM | KMOD_CAPS ) ) == 0 )
        {
#ifdef MAXDOF_CLIENT_BUILD
            if( mShipController )
            {
                mTextOverleyManager.mDisplayHelpMode = ( mTextOverleyManager.mDisplayHelpMode + 1 ) % mTextOverleyManager.mNumDisplayHelpModes;
                const float timeSinceLast = 0.0f;
                auto telemetry = mShipController->getTelemetry( timeSinceLast );
                mTextOverleyManager.generateDebugText( timeSinceLast, telemetry );
            }
#endif
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F1 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_PBS );
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F2 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_UNLIT );
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F3 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            Ogre::Root *root = mGraphicsSystem->getRoot();
            Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

            Ogre::Hlms *hlms = hlmsManager->getComputeHlms();
            Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
            hlms->reloadFrom( hlms->getDataFolder() );
        }
        else if( arg.keysym.scancode == SDL_SCANCODE_F5 &&
                 ( arg.keysym.mod & ( KMOD_LCTRL | KMOD_RCTRL ) ) )
        {
            Ogre::Root *root = mGraphicsSystem->getRoot();
            root->getRenderSystem()->validateDevice( true );
        }
        else
        {
            mKeyboardController.keyReleased( arg );
        }
    }

    void MdfGame::mouseMoved( const SDL_Event &arg )
    {
        mMouseController.mouseMoved( arg );
    }
}
