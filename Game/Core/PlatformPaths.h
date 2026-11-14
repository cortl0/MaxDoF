// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgreString.h>

namespace MaxDoF::Game::Core
{
    class PlatformPaths final
    {
    public:
        static Ogre::String getResourcePath();
        static Ogre::String getWriteAccessFolder();
    };
}

