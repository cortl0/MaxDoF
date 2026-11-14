// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgrePrerequisites.h>

namespace MaxDoF::Game::Core
{
    class OgreResourceSetup final
    {
    public:
        static void setupResources( const Ogre::String &resourcePath );
        static void registerHlms( Ogre::Root *root, const Ogre::String &resourcePath );
    };
}
