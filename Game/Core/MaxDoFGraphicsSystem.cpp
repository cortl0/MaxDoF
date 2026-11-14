// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "MaxDoFGraphicsSystem.h"
#include "PlatformPaths.h"
#include "OgreResourceSetup.h"

#include <fstream>
#include <OgreAbiUtils.h>
#include <OgreCamera.h>
#include <OgreConfigFile.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreWindow.h>
#include <Compositor/OgreCompositorManager2.h>
#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>
#include <OgrePlatformInformation.h>

namespace MaxDoF::Game::Core
{
    MaxDoFGraphicsSystem::MaxDoFGraphicsSystem()
        : mResourcePath( PlatformPaths::getResourcePath() )
        , mWriteAccessFolder( PlatformPaths::getWriteAccessFolder() )
    {
    }

    MaxDoFGraphicsSystem::~MaxDoFGraphicsSystem()
    {
        deinitialize();
    }

    void MaxDoFGraphicsSystem::initialize( const Ogre::String &windowTitle )
    {
        Ogre::String pluginsPath = mResourcePath + "plugins.cfg";
#if defined( DEBUG ) || defined( _DEBUG ) || defined( OGRE_DEBUG_MODE )
        std::ifstream debugCfg( ( mResourcePath + "plugins_d.cfg" ).c_str() );
        if( debugCfg.good() )
            pluginsPath = mResourcePath + "plugins_d.cfg";
#endif
        const Ogre::AbiCookie abiCookie = Ogre::generateAbiCookie();
        mRoot = OGRE_NEW Ogre::Root( &abiCookie,
                                     pluginsPath,
                                     mWriteAccessFolder + "ogre.cfg",
                                     mWriteAccessFolder + "Ogre.log" );

        if( !mRoot->restoreConfig() && !mRoot->showConfigDialog() )
        {
            OGRE_EXCEPT( Ogre::Exception::ERR_INVALID_STATE,
                         "Render system configuration was cancelled.",
                         "MaxDoFGraphicsSystem::initialize" );
        }

        mRoot->initialise( false, windowTitle );

        Ogre::ConfigOptionMap &cfgOpts = mRoot->getRenderSystem()->getConfigOptions();
        int width = 1280;
        int height = 720;

        auto optIt = cfgOpts.find( "Video Mode" );
        if( optIt != cfgOpts.end() && !optIt->second.currentValue.empty() )
        {
            const auto pos = optIt->second.currentValue.find( 'x' );
            if( pos != Ogre::String::npos )
            {
                width = Ogre::StringConverter::parseInt( optIt->second.currentValue.substr( 0, pos ) );
                height = Ogre::StringConverter::parseInt( optIt->second.currentValue.substr( pos + 1 ) );
            }
        }

        optIt = cfgOpts.find( "Full Screen" );
        const bool fullscreen = ( optIt != cfgOpts.end() && optIt->second.currentValue == "Yes" );

        mWindowManager.createWindow( windowTitle, width, height, fullscreen );

        Ogre::NameValuePairList params;
        params["externalWindowHandle"] = mWindowManager.getExternalWindowHandle();
        params["title"] = windowTitle;
        params["gamma"] = cfgOpts["sRGB Gamma Conversion"].currentValue;
        params["FSAA"] = cfgOpts["FSAA"].currentValue;
        params["vsync"] = cfgOpts["VSync"].currentValue;

        mRenderWindow = mRoot->createRenderWindow( windowTitle, width, height, fullscreen, &params );

        OgreResourceSetup::setupResources( mResourcePath );
        OgreResourceSetup::registerHlms( mRoot, mResourcePath );
        mOverlaySystem = OGRE_NEW Ogre::v1::OverlaySystem();
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups( true );

        createSceneManager();
        createCamera();
        mWorkspace = setupCompositor();

        mWindowManager.setRelativeMouseMode( true );
    }

    void MaxDoFGraphicsSystem::deinitialize()
    {
        if( mWorkspace )
        {
            mRoot->getCompositorManager2()->removeWorkspace( mWorkspace );
            mWorkspace = nullptr;
        }

        if( mSceneManager )
        {
            if( mOverlaySystem )
                mSceneManager->removeRenderQueueListener( mOverlaySystem );
            mRoot->destroySceneManager( mSceneManager );
            mSceneManager = nullptr;
        }

        OGRE_DELETE mOverlaySystem;
        mOverlaySystem = nullptr;

        if( mRoot )
        {
            OGRE_DELETE mRoot;
            mRoot = nullptr;
        }

        mWindowManager.destroyWindow();
    }

    void MaxDoFGraphicsSystem::createSceneManager()
    {
        const size_t numThreads = std::max<size_t>( 1, Ogre::PlatformInformation::getNumLogicalCores() );
        mSceneManager = mRoot->createSceneManager( Ogre::ST_GENERIC, numThreads, "MaxDoFSceneManager" );
        mSceneManager->addRenderQueueListener( mOverlaySystem );
        mSceneManager->getRenderQueue()->setSortRenderQueue(
            Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId,
            Ogre::RenderQueue::StableSort );
    }

    void MaxDoFGraphicsSystem::createCamera()
    {
        mCamera = mSceneManager->createCamera( "Main Camera" );
        mCamera->setPosition( Ogre::Vector3( 0, 5, 15 ) );
        mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
        mCamera->setNearClipDistance( 0.05f );
        mCamera->setFarClipDistance( 10000.0f );
        mCamera->setAutoAspectRatio( true );
    }

    Ogre::CompositorWorkspace *MaxDoFGraphicsSystem::setupCompositor()
    {
        Ogre::CompositorManager2 *compositorManager = mRoot->getCompositorManager2();
        const Ogre::String workspaceName( "MaxDoFWorkspace" );
        if( !compositorManager->hasWorkspaceDefinition( workspaceName ) )
            compositorManager->createBasicWorkspaceDef( workspaceName, mBackgroundColour, Ogre::IdString() );

        return compositorManager->addWorkspace( mSceneManager, mRenderWindow->getTexture(), mCamera, workspaceName, true );
    }
}
