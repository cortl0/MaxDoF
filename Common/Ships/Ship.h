// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreCamera.h>
#include "Game/Ships/Dashboard.h"
#endif

#include "Common/Types/Telemetry.h"
#include "Common/World/Manager.h"

namespace MaxDoF::Common::Ships
{
    class Ship {
        World::Manager& mWorldManager;
#ifdef MAXDOF_CLIENT_BUILD
        Game::Ships::Dashboard mDashboard;
        Ogre::SceneNode* lightNode;
        Ogre::Light* light;
#endif
        Types::Entity mEntity;

    public:
        Ship(
            World::Manager&,
#ifdef MAXDOF_CLIENT_BUILD
            Game::World::Graphics&,
#endif
            const Types::Entity&);
        ~Ship();
#ifdef MAXDOF_CLIENT_BUILD
        void connectCameraToShip(Ogre::Camera*);
#endif
        const Types::Entity& getEntity() const { return mEntity; }
        void update(float timeSinceLastFrame, const Types::Telemetry&);
        void CreateBullet(float timeSinceLast, bool needCreate);
    };
}
