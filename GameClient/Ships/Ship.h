// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#pragma once

#ifdef MAXDOF_CLIENT_BUILD
#include "GameClient/Ships/Dashboard.h"
#endif

#include "GameCommon/Types/Entity.h"
#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient::Ships
{
    class Ship {
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneManager* mSceneMgr;
        Dashboard mDashboard;
#endif
        GameCommon::Types::Entity mEntity;

    public:
        Ship(
#ifdef MAXDOF_CLIENT_BUILD
            Ogre::SceneManager* sceneMgr,
#endif
            const GameCommon::Types::Entity& shipEntity);
        ~Ship();
        const GameCommon::Types::Entity& getEntity() const { return mEntity; }
        void update(float timeSinceLastFrame, const GameCommon::Types::Telemetry&);
    };
}
