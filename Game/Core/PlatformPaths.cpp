// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "PlatformPaths.h"

#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#    include <cerrno>
#    include <cstdio>
#    include <cstdlib>
#    include <pwd.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <unistd.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    include <shlobj.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#    include <Foundation/Foundation.h>
#    include "OSX/macUtils.h"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#    include <Foundation/Foundation.h>
#    include "iOS/macUtils.h"
#endif

namespace MaxDoF::Game::Core
{
    Ogre::String PlatformPaths::getResourcePath()
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
        return "Data/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        return Ogre::macBundlePath() + "/Contents/Resources/Data/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        return Ogre::macBundlePath() + "/Data/";
#else
        return "../Data/";
#endif
    }

    Ogre::String PlatformPaths::getWriteAccessFolder()
    {
        Ogre::String writeFolder;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        writeFolder = "/";
        TCHAR path[MAX_PATH];
        if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path ) ) )
        {
#    if defined( _UNICODE ) || defined( UNICODE )
            int size_needed = WideCharToMultiByte( CP_OEMCP, 0, path, (int)wcslen( path ), NULL, 0, NULL, NULL );
            writeFolder = std::string( size_needed, 0 );
            WideCharToMultiByte( CP_OEMCP, 0, path, (int)wcslen( path ), &writeFolder[0], size_needed, NULL, NULL );
#    else
            TCHAR oemPath[MAX_PATH];
            CharToOem( path, oemPath );
            writeFolder = std::string( oemPath );
#    endif
            writeFolder += "/MaxDoF/";

            if( !CreateDirectoryA( writeFolder.c_str(), NULL ) &&
                GetLastError() != ERROR_ALREADY_EXISTS )
            {
                writeFolder = "";
            }
        }
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        const char *homeDir = getenv( "HOME" );
        if( homeDir == 0 )
            homeDir = getpwuid( getuid() )->pw_dir;
        writeFolder = homeDir;
        writeFolder += "/.config";
        int result = mkdir( writeFolder.c_str(), S_IRWXU | S_IRWXG );
        int errorReason = errno;

        if( result && errorReason != EEXIST )
        {
            printf( "Error. Failing to create path '%s'. Do you have access rights?", writeFolder.c_str() );
            writeFolder = "";
        }
        else
        {
            writeFolder += "/MaxDoF/";
            result = mkdir( writeFolder.c_str(), S_IRWXU | S_IRWXG );
            errorReason = errno;

            if( result && errorReason != EEXIST )
            {
                printf( "Error. Failing to create path '%s'. Do you have access rights?", writeFolder.c_str() );
                writeFolder = "";
            }
        }
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        NSURL *libUrl = [NSFileManager.defaultManager URLForDirectory:NSLibraryDirectory
                                                             inDomain:NSUserDomainMask
                                                    appropriateForURL:nil
                                                               create:YES
                                                                error:nil];
        writeFolder = Ogre::String( libUrl.absoluteURL.path.UTF8String ) + "/MaxDoF/";
        const int result = mkdir( writeFolder.c_str(), S_IRWXU | S_IRWXG );
        const int errorReason = errno;

        if( result && errorReason != EEXIST )
        {
            printf( "Error. Failing to create path '%s'. Do you have access rights?", writeFolder.c_str() );
            writeFolder = "";
        }
#endif
        return writeFolder;
    }
}

