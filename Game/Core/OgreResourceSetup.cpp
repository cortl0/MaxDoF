// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "OgreResourceSetup.h"

#include <OgreArchiveManager.h>
#include <OgreConfigFile.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>
#include <OgreRenderSystem.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>

namespace MaxDoF::Game::Core
{
    void OgreResourceSetup::setupResources( const Ogre::String &resourcePath )
    {
        Ogre::ConfigFile cf;
        cf.load( resourcePath + "resources2.cfg" );

        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
        while( seci.hasMoreElements() )
        {
            Ogre::String secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();

            if( secName != "Hlms" )
            {
                for( auto &pair : *settings )
                {
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                        pair.second, pair.first, secName );
                }
            }
        }

        Ogre::String dataFolder = cf.getSetting( "DoNotUseAsResource", "Hlms", "" );
        if( dataFolder.empty() )
            dataFolder = "./";
        else if( dataFolder.back() != '/' )
            dataFolder += "/";

        dataFolder += "2.0/scripts/materials/PbsMaterials";
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation( dataFolder, "FileSystem", "General" );
    }

    void OgreResourceSetup::registerHlms( Ogre::Root *root, const Ogre::String &resourcePath )
    {
        Ogre::ConfigFile cf;
        cf.load( resourcePath + "resources2.cfg" );

        Ogre::String rootHlmsFolder = cf.getSetting( "DoNotUseAsResource", "Hlms", "" );
        if( rootHlmsFolder.empty() )
            rootHlmsFolder = resourcePath;
        else if( rootHlmsFolder.back() != '/' )
            rootHlmsFolder += "/";

        Ogre::String mainFolderPath;
        Ogre::StringVector libraryFoldersPaths;
        Ogre::ArchiveManager &archiveManager = Ogre::ArchiveManager::getSingleton();
        Ogre::HlmsManager *hlmsManager = root->getHlmsManager();

        Ogre::HlmsUnlit *hlmsUnlit = nullptr;
        Ogre::HlmsPbs *hlmsPbs = nullptr;

        if( !hlmsManager->getHlms( Ogre::HLMS_UNLIT ) )
        {
            Ogre::HlmsUnlit::getDefaultPaths( mainFolderPath, libraryFoldersPaths );
            Ogre::Archive *archiveUnlit =
                archiveManager.load( rootHlmsFolder + mainFolderPath, "FileSystem", true );

            Ogre::ArchiveVec archiveUnlitLibraryFolders;
            for( const auto &libPath : libraryFoldersPaths )
            {
                Ogre::Archive *archiveLibrary =
                    archiveManager.load( rootHlmsFolder + libPath, "FileSystem", true );
                archiveUnlitLibraryFolders.push_back( archiveLibrary );
            }

            hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit( archiveUnlit, &archiveUnlitLibraryFolders );
            hlmsManager->registerHlms( hlmsUnlit );
        }

        if( !hlmsManager->getHlms( Ogre::HLMS_PBS ) )
        {
            Ogre::HlmsPbs::getDefaultPaths( mainFolderPath, libraryFoldersPaths );
            Ogre::Archive *archivePbs =
                archiveManager.load( rootHlmsFolder + mainFolderPath, "FileSystem", true );

            Ogre::ArchiveVec archivePbsLibraryFolders;
            for( const auto &libPath : libraryFoldersPaths )
            {
                Ogre::Archive *archiveLibrary =
                    archiveManager.load( rootHlmsFolder + libPath, "FileSystem", true );
                archivePbsLibraryFolders.push_back( archiveLibrary );
            }

            hlmsPbs = OGRE_NEW Ogre::HlmsPbs( archivePbs, &archivePbsLibraryFolders );
            hlmsManager->registerHlms( hlmsPbs );
        }

        Ogre::RenderSystem *renderSystem = root->getRenderSystem();
        if( renderSystem->getName() == "Direct3D11 Rendering Subsystem" )
        {
            bool supportsNoOverwriteOnTextureBuffers = false;
            renderSystem->getCustomAttribute( "MapNoOverwriteOnDynamicBufferSRV",
                                              &supportsNoOverwriteOnTextureBuffers );

            if( !supportsNoOverwriteOnTextureBuffers )
            {
                if( hlmsPbs )
                    hlmsPbs->setTextureBufferDefaultSize( 512 * 1024 );
                if( hlmsUnlit )
                    hlmsUnlit->setTextureBufferDefaultSize( 512 * 1024 );
            }
        }
    }
}
