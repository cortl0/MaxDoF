// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#include <vector>

#include <OgreSceneManager.h>

#include "GameClient/World/Graphics.h"
#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient::Ships
{
    class Dashboard {
        Ogre::SceneManager* mSceneMgr;
        GameClient::World::Graphics& mGraphics;
        Ogre::SceneNode* mDashboardNode; // Корневой узел панели

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
        Dashboard(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode, const Ogre::Vector3& position, GameClient::World::Graphics&);
        ~Dashboard();
        void update(const GameCommon::Types::Telemetry&);
    };
}
