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

#include "Common/Scenes/SceneRace.h"
#include "Common/Scenes/SceneTest.h"
#include "Common/Utils/SystemUtils.h"

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
        mShipController = std::make_unique<Ships::ShipController>( mScene->GetShip(), mWorldManager.getBodyInterface() );
    }

    void MdfGame::createScene01()
    {
        const int scene{1};
        switch (scene)
        {
        case 0:
            mScene = std::make_unique<Common::Scenes::SceneTest>(mWorldManager);
            break;
        case 1:
            mScene = std::make_unique<Common::Scenes::SceneRace>(mWorldManager);
            break;
        
        default:
            MaxDoF_LOG_AND_ABORT("scene not found")
            break;
        }
        mWorldManager.initialize( mMaxBodiesLimit );
        mScene->CreateScene();
#ifdef MAXDOF_CLIENT_BUILD
        mTextOverleyManager.createCrosshair();
        mTextOverleyManager.createDebugTextOverlay();
#endif
        mScene->GetShip().connectCameraToShip( mGraphicsSystem->getCamera() );
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

    Ogre::String GetKeysString(const Ships::ShipController::ControllerData& d)
    {
        Ogre::String s;

        {
            if(d.moveDir.y > 0.0f)
                s += " SPACE\n";
            else
                s += " space\n";

            if(d.moveDir.z < 0.0f)
                s += "   W\n";
            else
                s += "   w\n";

            if(d.moveDir.x < 0.0f)
                s += " A";
            else
                s += " a";

            if(d.moveDir.z > 0.0f)
                s += " S";
            else
                s += " s";

            if(d.moveDir.x > 0.0f)
                s += " D\n";
            else
                s += " d\n";

            if(d.moveDir.y < 0.0f)
                s += " SHIFT\n";
            else
                s += " shift\n";
        }

        {
            if(d.leftMouseButtonPressed)
                s += "LBM ";
            else
                s += "lbm ";

            if(d.rightMouseButtonPressed)
                s += "RBM\n";
            else
                s += "rbm\n";
        }

        {
            constexpr int coef_dx{16};
            constexpr int coef_dy{8};
            constexpr int size{5};
            s += "m_dx [";

            for(int i = -size; i <= +size; ++i)
                if(static_cast<int>(d.mouse_dx) / coef_dx < i && i < 0)
                    s += "*";
                else if(static_cast<int>(d.mouse_dx) / coef_dx > i && i > 0)
                    s += "*";
                else
                s += " ";

            s += "]\nm_dy [";

            for(int i = -size; i <= +size; ++i)
                if(static_cast<int>(-d.mouse_dy) / coef_dy < i && i < 0)
                    s += "*";
                else if(static_cast<int>(-d.mouse_dy) / coef_dy > i && i > 0)
                    s += "*";
                else
                s += " ";

            s += "]\n";
        }
        return s;
    }

    void MdfGame::update( float timeSinceLast )
    {
        auto data = mInputTranslator.translate( &mKeyboardController, &mMouseController );
        mMouseController.reset();

        if( mShipController )
            mShipController->update( timeSinceLast, data );

        mWorldManager.update( timeSinceLast, mScene->GetShip().getEntity().physId );
        mScene->Update();

#ifdef MAXDOF_CLIENT_BUILD
        if( mTextOverleyManager.mDisplayHelpMode != 0 )
        {
            auto telemetry = mShipController->getTelemetry( timeSinceLast );
            Ogre::String keyPressed = GetKeysString(data);
            mTextOverleyManager.generateDebugText(timeSinceLast, telemetry, keyPressed);
            mScene->GetShip().update( timeSinceLast, telemetry );
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
                mTextOverleyManager.generateDebugText(timeSinceLast, telemetry, {});
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
