// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "MaxDoFGraphicsSystem.h"

#include <Compositor/OgreCompositorManager2.h>
#include <OgreConfigFile.h>
#include <OgreRoot.h>
#include <OgreWindow.h>
#include <OgrePrerequisites.h>
#include <OgreString.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#    include <errno.h>
#    include <pwd.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <unistd.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    include "shlobj.h"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#    include <Foundation/Foundation.h>
#    include "OSX/macUtils.h"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#    include <Foundation/Foundation.h>
#    include "iOS/macUtils.h"
#endif

namespace MaxDoF::Game
{    class PlatformPaths final
    {
    public:
        static Ogre::String getResourcePath();
        static Ogre::String getWriteAccessFolder();
    };
    MaxDoFGraphicsSystem::MaxDoFGraphicsSystem( Demo::GameState *gameState ) :
        Demo::GraphicsSystem( gameState )
    {
        mResourcePath = PlatformPaths::getResourcePath();
        mWriteAccessFolder = PlatformPaths::getWriteAccessFolder();
    }

    Ogre::CompositorWorkspace *MaxDoFGraphicsSystem::setupCompositor()
    {
        Ogre::CompositorManager2 *compositorManager = mRoot->getCompositorManager2();

        const Ogre::String workspaceName( "MaxDoFWorkspace" );
        if( !compositorManager->hasWorkspaceDefinition( workspaceName ) )
        {
            compositorManager->createBasicWorkspaceDef( workspaceName, mBackgroundColour, Ogre::IdString() );
        }

        return compositorManager->addWorkspace( mSceneManager, mRenderWindow->getTexture(), mCamera, workspaceName, true );
    }

    void MaxDoFGraphicsSystem::setupResources()
    {
        GraphicsSystem::setupResources();

        Ogre::ConfigFile cf;
        cf.load( mResourcePath + "resources2.cfg" );

        Ogre::String dataFolder = cf.getSetting( "DoNotUseAsResource", "Hlms", "" );

        if( dataFolder.empty() )
            dataFolder = "./";
        else if( *( dataFolder.end() - 1 ) != '/' )
            dataFolder += "/";

        dataFolder += "2.0/scripts/materials/PbsMaterials";

        addResourceLocation( dataFolder, "FileSystem", "General" );
    }
/*
    void MaxDoFGraphicsSystem::initWriteAccessFolder()
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        mWriteAccessFolder = "/";
        TCHAR path[MAX_PATH];
        if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path ) != S_OK ) )
        {
#    if defined( _UNICODE ) || defined( UNICODE )
            int size_needed = WideCharToMultiByte( CP_OEMCP, 0, path, (int)wcslen( path ), NULL, 0, NULL, NULL );
            mWriteAccessFolder = std::string( size_needed, 0 );
            WideCharToMultiByte( CP_OEMCP, 0, path, (int)wcslen( path ), &mWriteAccessFolder[0], size_needed, NULL, NULL );
#    else
            TCHAR oemPath[MAX_PATH];
            CharToOem( path, oemPath );
            mWriteAccessFolder = std::string( oemPath );
#    endif
            mWriteAccessFolder += "/MaxDoF/";

            if( !CreateDirectoryA( mWriteAccessFolder.c_str(), NULL ) &&
                GetLastError() != ERROR_ALREADY_EXISTS )
            {
                mWriteAccessFolder = "";
            }
        }
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        const char *homeDir = getenv( "HOME" );
        if( homeDir == 0 )
            homeDir = getpwuid( getuid() )->pw_dir;
        mWriteAccessFolder = homeDir;
        mWriteAccessFolder += "/.config";
        int result = mkdir( mWriteAccessFolder.c_str(), S_IRWXU | S_IRWXG );
        int errorReason = errno;

        if( result && errorReason != EEXIST )
        {
            printf( "Error. Failing to create path '%s'. Do you have access rights?", mWriteAccessFolder.c_str() );
            mWriteAccessFolder = "";
        }
        else
        {
            mWriteAccessFolder += "/MaxDoF/";
            result = mkdir( mWriteAccessFolder.c_str(), S_IRWXU | S_IRWXG );
            errorReason = errno;

            if( result && errorReason != EEXIST )
            {
                printf( "Error. Failing to create path '%s'. Do you have access rights?", mWriteAccessFolder.c_str() );
                mWriteAccessFolder = "";
            }
        }
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        NSURL *libUrl = [NSFileManager.defaultManager URLForDirectory:NSLibraryDirectory
                                                             inDomain:NSUserDomainMask
                                                    appropriateForURL:nil
                                                               create:YES
                                                                error:nil];
        mWriteAccessFolder = Ogre::String( libUrl.absoluteURL.path.UTF8String ) + "/MaxDoF/";
        const int result = mkdir( mWriteAccessFolder.c_str(), S_IRWXU | S_IRWXG );
        const int errorReason = errno;

        if( result && errorReason != EEXIST )
        {
            printf( "Error. Failing to create path '%s'. Do you have access rights?", mWriteAccessFolder.c_str() );
            mWriteAccessFolder = "";
        }
#endif
    }
*/
}
