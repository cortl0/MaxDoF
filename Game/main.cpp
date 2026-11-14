// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include <exception>
#include <iostream>
#include <OgreException.h>

#include "version.h"
#include "Game/Application.h"

int main( int argc, char *argv[] )
{
    std::cout << "MaxDoF Game Version: " << MaxDoF_VERSION_STRING << std::endl;

    try
    {
        MaxDoF::Game::Application app;
        app.run( "MaxDoF", "MaxDoF Client" );
    }
    catch( const Ogre::Exception &e )
    {
        std::cerr << "Ogre exception: " << e.getFullDescription() << std::endl;
        return 1;
    }
    catch( const std::exception &e )
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
