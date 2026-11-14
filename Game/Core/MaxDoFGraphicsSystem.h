// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <OgrePrerequisites.h>
#include <OgreColourValue.h>

#include "SdlWindowManager.h"

namespace Ogre::v1
{
    class OverlaySystem;
}

namespace MaxDoF::Game::Core
{
    class MaxDoFGraphicsSystem final
    {
        Ogre::Root *mRoot{ nullptr };
        Ogre::Window *mRenderWindow{ nullptr };
        Ogre::SceneManager *mSceneManager{ nullptr };
        Ogre::Camera *mCamera{ nullptr };
        Ogre::CompositorWorkspace *mWorkspace{ nullptr };
        Ogre::v1::OverlaySystem *mOverlaySystem{ nullptr };

        SdlWindowManager mWindowManager;
        Ogre::String mResourcePath;
        Ogre::String mWriteAccessFolder;
        Ogre::ColourValue mBackgroundColour{ 0.0f, 0.0f, 0.0f, 1.0f };

        void createSceneManager();
        void createCamera();
        Ogre::CompositorWorkspace *setupCompositor();

    public:
        MaxDoFGraphicsSystem();
        ~MaxDoFGraphicsSystem();

        void initialize( const Ogre::String &windowTitle );
        void deinitialize();

        Ogre::Root *getRoot() const { return mRoot; }
        Ogre::Window *getRenderWindow() const { return mRenderWindow; }
        Ogre::SceneManager *getSceneManager() const { return mSceneManager; }
        Ogre::Camera *getCamera() const { return mCamera; }
        Ogre::CompositorWorkspace *getWorkspace() const { return mWorkspace; }
    };
}
