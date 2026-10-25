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
        lightNode = mEntity.graphNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        // lightNode->setPosition(Ogre::Vector3(0.0f, 0.0f, -0.4f));
        light = mSceneMgr->createLight();
        light->setType(Ogre::Light::LT_SPOTLIGHT);
        constexpr float lightRadius = 100.0f;
        light->setAttenuation(lightRadius, 1.0f, 0.0f, 0.01f);
        light->setSpotlightRange(Ogre::Degree(0), Ogre::Degree(60));
        lightNode->attachObject(light);
    }

    Ship::~Ship()
    {
        mSceneMgr->destroyLight(light);
        mSceneMgr->destroySceneNode(lightNode);
    }

    void Ship::update(float timeSinceLastFrame, const GameCommon::Types::Telemetry& telemetry)
    {
        mDashboard.update(telemetry);
    }
}
