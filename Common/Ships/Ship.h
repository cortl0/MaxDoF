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

#ifdef MAXDOF_CLIENT_BUILD
#include "Game/Ships/ControllerData.h"
#endif

namespace MaxDoF::Common::Ships
{
    class Ship {
        World::Manager& mWorldManager;
        Types::Entity mEntity;
#ifdef MAXDOF_CLIENT_BUILD
        Game::Ships::Dashboard mDashboard;
        Ogre::SceneNode* lightNode;
        Ogre::Light* light;
#endif
        int numBullets{250};

    public:
        Ship(World::Manager&);
        ~Ship();
#ifdef MAXDOF_CLIENT_BUILD
        void connectCameraToShip(Ogre::Camera*);
#endif
        const Types::Entity& getEntity() const { return mEntity; }
#ifdef MAXDOF_CLIENT_BUILD
        void update(float timeSinceLastFrame, const Types::Telemetry&, const Game::Ships::ControllerData&);
#endif
        void CreateBullet(float timeSinceLast, bool needCreate);
        Types::Entity& createShipEntity();
    };
}
