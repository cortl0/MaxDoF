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

namespace MaxDoF::Game
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
            rootHlmsFolder = "./";
        else if( rootHlmsFolder.back() != '/' )
            rootHlmsFolder += "/";

        Ogre::RenderSystem *renderSystem = root->getRenderSystem();
        Ogre::String shaderSyntax = "GLSL";
        if( renderSystem->getName() == "Direct3D11 Rendering Subsystem" )
            shaderSyntax = "HLSL";
        else if( renderSystem->getName() == "Metal Rendering Subsystem" )
            shaderSyntax = "Metal";
        else if( renderSystem->getName() == "Vulkan Rendering Subsystem" )
            shaderSyntax = "GLSLVK";

        auto getArchiveVec = [&]( const Ogre::String &libraryPath ) -> Ogre::ArchiveVec {
            return {
                Ogre::ArchiveManager::getSingletonPtr()->load( rootHlmsFolder + libraryPath, "FileSystem", true ),
                Ogre::ArchiveManager::getSingletonPtr()->load( rootHlmsFolder + "Hlms/Common/Any", "FileSystem", true ),
                Ogre::ArchiveManager::getSingletonPtr()->load( rootHlmsFolder + "Hlms/Common/" + shaderSyntax, "FileSystem", true )
            };
        };

        Ogre::ArchiveVec unlitLibs = getArchiveVec( "Hlms/Unlit/Any" );
        Ogre::Archive *archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(
            rootHlmsFolder + "Hlms/Unlit/" + shaderSyntax, "FileSystem", true );
        root->getHlmsManager()->registerHlms( OGRE_NEW Ogre::HlmsUnlit( archiveUnlit, &unlitLibs ) );

        Ogre::ArchiveVec pbsLibs = getArchiveVec( "Hlms/Pbs/Any" );
        Ogre::Archive *archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
            rootHlmsFolder + "Hlms/Pbs/" + shaderSyntax, "FileSystem", true );
        root->getHlmsManager()->registerHlms( OGRE_NEW Ogre::HlmsPbs( archivePbs, &pbsLibs ) );
    }
}
