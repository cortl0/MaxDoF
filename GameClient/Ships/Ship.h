// https://Github/cortl0/MaxDoF
// GPL-3.0 license

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
