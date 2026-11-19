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
        const Ogre::AbiCookie abiCookie = Ogre::generateAbiCookie();

        // 1. Создаем Root БЕЗ автоматической жесткой загрузки plugins.cfg (передаем "")
        mRoot = new Ogre::Root(
            &abiCookie,
            "",
            mWriteAccessFolder + "ogre.cfg",
            mWriteAccessFolder + "Ogre.log"
        );

        // 2. Читаем plugins.cfg / plugins_d.cfg вручную и загружаем плагины как опциональные (bOptional = true)
        Ogre::String pluginsPath = mResourcePath + "plugins.cfg";
        std::ifstream debugCfg( ( mResourcePath + "plugins_d.cfg" ).c_str() );
        if( debugCfg.good() )
        {
            pluginsPath = mResourcePath + "plugins_d.cfg";
        }

        Ogre::ConfigFile cfg;
        cfg.load( pluginsPath );

        Ogre::String pluginDir = cfg.getSetting( "PluginFolder" );
        if( !pluginDir.empty() && pluginDir.back() != '/' && pluginDir.back() != '\\' )
        {
            pluginDir += "/";
        }

        const Ogre::StringVector &plugins = cfg.getMultiSetting( "Plugin" );
        for( const Ogre::String &pluginName : plugins )
        {
            // Второй параметр true = bOptional: если DLL или vulkan-1.dll не найдены, OGRE не упадет
            mRoot->loadPlugin( pluginDir + pluginName, true, nullptr );
        }

        if( !mRoot->restoreConfig() )
        {
        #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            const Ogre::RenderSystemList &renderers = mRoot->getAvailableRenderers();
            if( renderers.empty() )
            {
                OGRE_EXCEPT( Ogre::Exception::ERR_RENDERINGAPI_ERROR,
                            "No render systems available!",
                            "MaxDoFGraphicsSystem::initialize" );
            }

            // Ищем предпочтительный рендер: сначала OpenGL 3+ (для максимальной совместимости), затем Vulkan
            Ogre::RenderSystem *selectedRenderer = nullptr;
            for( Ogre::RenderSystem *rs : renderers )
            {
                if( rs->getName().find( "OpenGL 3+" ) != Ogre::String::npos )
                {
                    selectedRenderer = rs;
                    break;
                }
            }

            if( !selectedRenderer )
            {
                for( Ogre::RenderSystem *rs : renderers )
                {
                    if( rs->getName().find( "Vulkan" ) != Ogre::String::npos )
                    {
                        selectedRenderer = rs;
                        break;
                    }
                }
            }

            if( !selectedRenderer )
            {
                selectedRenderer = renderers.front();
            }

            mRoot->setRenderSystem( selectedRenderer );

            // Выставляем безопасные настройки по умолчанию
            const Ogre::ConfigOptionMap &options = selectedRenderer->getConfigOptions();

            if( options.find( "Full Screen" ) != options.end() )
                selectedRenderer->setConfigOption( "Full Screen", "Yes" );

            if( options.find( "VSync" ) != options.end() )
                selectedRenderer->setConfigOption( "VSync", "Yes" );

            // Подбираем разрешение 1280x720 из списка доступных у выбранного рендера
            auto videoModeIt = options.find( "Video Mode" );
            if( videoModeIt != options.end() )
            {
                for( const Ogre::String &mode : videoModeIt->second.possibleValues )
                {
                    if( mode.find( "1920 x 1080" ) != Ogre::String::npos )
                    {
                        selectedRenderer->setConfigOption( "Video Mode", mode );
                        break;
                    }
                }
            }

            mRoot->saveConfig();
        #else
            if( !mRoot->showConfigDialog() )
            {
                OGRE_EXCEPT( Ogre::Exception::ERR_INVALID_STATE,
                            "Render system configuration was cancelled.",
                            "MaxDoFGraphicsSystem::initialize" );
            }
        #endif
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
