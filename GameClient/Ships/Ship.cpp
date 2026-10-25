// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Ships/Ship.h"

#include <OgreItem.h>

namespace MaxDoF::GameClient::Ships
{
    Ship::Ship(Ogre::SceneManager* sceneMgr, const GameCommon::Types::Entity& shipEntity)
        : mSceneMgr(sceneMgr)
        , mDashboard(mSceneMgr, shipEntity.graphNode, Ogre::Vector3(0.0f, -0.1f, -0.3f))
        , mEntity(shipEntity)
    {
    }

    Ship::~Ship()
    {
    }

    void Ship::update(float timeSinceLastFrame, const GameCommon::Types::Telemetry& telemetry)
    {
        mDashboard.update(telemetry);
    }
}
