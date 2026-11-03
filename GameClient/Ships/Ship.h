// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreCamera.h>
#include "GameClient/Ships/Dashboard.h"
#endif

#include "GameCommon/Types/Entity.h"
#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient::Ships
{
    class Ship {
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneManager* mSceneMgr;
        GameClient::Ships::Dashboard mDashboard;
        Ogre::SceneNode* lightNode;
        Ogre::Light* light;
#endif
        GameCommon::Types::Entity mEntity;

    public:
        Ship(
#ifdef MAXDOF_CLIENT_BUILD
            Ogre::SceneManager*,
            GameClient::World::Graphics&,
#endif
            const GameCommon::Types::Entity& shipEntity);
        ~Ship();
#ifdef MAXDOF_CLIENT_BUILD
        void connectCameraToShip(Ogre::Camera*);
#endif
        const GameCommon::Types::Entity& getEntity() const { return mEntity; }
        void update(float timeSinceLastFrame, const GameCommon::Types::Telemetry&);
    };
}
