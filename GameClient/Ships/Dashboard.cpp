// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Ships/Dashboard.h"

#include <Ogre.h>

namespace MaxDoF::GameClient::Ships
{
    Dashboard::Dashboard(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode, const Ogre::Vector3& position)
        : mSceneMgr(sceneMgr)
    {
        mDashboardNode = parentNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        mDashboardNode->setPosition(position);
        mDashboardNode->pitch(-Ogre::Radian(position.y / position.z));

        deskCube.node = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        deskCube.entity = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE, Ogre::SCENE_DYNAMIC);
        deskCube.node->attachObject(deskCube.entity);
        deskCube.node->setScale(0.0005f, 0.0003f, 0.00001f);

        gyroscopeCube.node = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        gyroscopeCube.entity = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE, Ogre::SCENE_DYNAMIC);
        gyroscopeCube.node->attachObject(gyroscopeCube.entity);
        gyroscopeCube.node->setScale(0.0001f, 0.0001f, 0.0001f);
        gyroscopeCube.node->setPosition(0.0f, 0.0f, 0.015f);
/*
        angularVelocityCube.node = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        angularVelocityCube.entity = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE, Ogre::SCENE_DYNAMIC);
        angularVelocityCube.node->attachObject(angularVelocityCube.entity);
        angularVelocityCube.node->setScale(0.0001f, 0.0001f, 0.0001f);
        angularVelocityCube.node->setPosition(-0.015f, 0.0f, 0.015f);
*/
        velocityCube.node = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        velocityCube.entity = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE, Ogre::SCENE_DYNAMIC);
        velocityCube.node->attachObject(velocityCube.entity);
        velocityCube.node->setScale(0.0001f, 0.0001f, 0.0001f);
        velocityCube.node->setPosition(0.015f, 0.0f, 0.015f);

        lightNode = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        lightNode->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.25f));
        light = mSceneMgr->createLight();
        light->setType(Ogre::Light::LT_POINT);
/*
        light->setDiffuseColour(Ogre::ColourValue(0.2f, 0.7f, 1.5f));
        light->setSpecularColour(Ogre::ColourValue(0.5f, 0.8f, 1.5f));
        light->setPowerScale(Ogre::Math::PI * 4.0f);
*/
        float lightRadius = 0.3f;
        light->setAttenuation(lightRadius, 1.0f, 0.0f, 0.0f);
        lightNode->attachObject(light);
    }

    Dashboard::~Dashboard() {
        mSceneMgr->destroyLight(light);
        mSceneMgr->destroySceneNode(lightNode);
        mSceneMgr->destroyEntity(velocityCube.entity);
        mSceneMgr->destroySceneNode(velocityCube.node);
/*
        mSceneMgr->destroyEntity(angularVelocityCube.entity);
        mSceneMgr->destroySceneNode(angularVelocityCube.node);
*/
        mSceneMgr->destroyEntity(gyroscopeCube.entity);
        mSceneMgr->destroySceneNode(gyroscopeCube.node);
        mSceneMgr->destroyEntity(deskCube.entity);
        mSceneMgr->destroySceneNode(deskCube.node);
        mSceneMgr->destroySceneNode(mDashboardNode);
    }
/*
    Ogre::Quaternion mirrorOrientation(const Ogre::Quaternion& srcOri, const Ogre::Quaternion& baseOri)
    {
        Ogre::Quaternion localOri = baseOri.Inverse() * srcOri;
        Ogre::Quaternion localMirrored(localOri.w, localOri.x, -localOri.y, -localOri.z);

        // 3. Переводим отзеркаленный локальный кватернион обратно в исходное пространство
        Ogre::Quaternion finalOri = baseOri * localMirrored;

        return finalOri;
    }
*/
    void Dashboard::update(const GameCommon::Types::Telemetry& telemetry)
    {
        auto shipOrientation = gyroscopeCube.node->getParent()->getParent()->getOrientation();
        gyroscopeCube.node->setOrientation(shipOrientation.Inverse());
        Ogre::Vector3 localVelocity(telemetry.localVelocity.x, telemetry.localVelocity.y, telemetry.localVelocity.z);
        float l = localVelocity.length();
        if(l>10.0f)
            localVelocity *= 10.0f / l;
        Ogre::Vector3 vec_(0.015f, 0.0f, 0.015f);
        velocityCube.node->setPosition(vec_ + localVelocity/2000.0f);
    }
}
