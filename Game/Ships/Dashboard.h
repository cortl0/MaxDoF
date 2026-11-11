// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>

#include <OgreSceneManager.h>

#include "Game/World/Graphics.h"
#include "Common/Types/Telemetry.h"

namespace MaxDoF::Game::Ships
{
    class Dashboard {
        Ogre::SceneManager* mSceneMgr;
        Game::World::Graphics& mGraphics;
        Ogre::SceneNode* mDashboardNode;

        struct Element
        {
            Ogre::SceneNode* node;
            Ogre::v1::Entity* entity;
        };
        Element gyroscopeCube;
        Element angularVelocityCube;
        Element velocityCube;
        Element deskCube;

/*
        Ogre::SceneNode* lightNode;
        Ogre::Light* light;
*/

    public:
        Dashboard(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode, const Ogre::Vector3& position, Game::World::Graphics&);
        ~Dashboard();
        void update(const Common::Types::Telemetry&);
    };
}
