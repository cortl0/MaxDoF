// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/Ships/Dashboard.h"

#include <Ogre.h>

namespace MaxDoF::GameClient::Ships
{
    Dashboard::Dashboard(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode, const Ogre::Vector3& position, GameClient::World::Graphics& graphics)
        : mSceneMgr(sceneMgr)
        , mGraphics(graphics)
    {
        mDashboardNode = parentNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        mDashboardNode->setPosition(position);
        mDashboardNode->pitch(-Ogre::Radian(position.y / position.z));

        deskCube.node = mGraphics.createCube(MaxDoF::GameCommon::Types::GraphPose{
            .position = {0.0f, 0.0f, 0.0f},
            .orientation = Ogre::Quaternion::IDENTITY
        }, Ogre::Vector3{0.5f, 0.5f, 0.5f}, mDashboardNode);
        deskCube.node->setScale(0.05f, 0.03f, 0.001f);

        gyroscopeCube.node = mGraphics.createCube(MaxDoF::GameCommon::Types::GraphPose{
            .position = {0.0f, 0.0f, 0.015f},
            .orientation = Ogre::Quaternion::IDENTITY
        }, Ogre::Vector3{0.5f, 0.5f, 0.5f}, mDashboardNode);
        gyroscopeCube.node->setScale(0.01f, 0.01f, 0.01f);

        angularVelocityCube.node = mGraphics.createSphere(MaxDoF::GameCommon::Types::GraphPose{
            .position = {-0.015f, 0.0f, 0.015f},
            .orientation = Ogre::Quaternion::IDENTITY
        }, 0.5f, mDashboardNode);
        angularVelocityCube.node->setScale(0.01f, 0.01f, 0.01f);

        velocityCube.node = mGraphics.createSphere(MaxDoF::GameCommon::Types::GraphPose{
            .position = {0.015f, 0.0f, 0.015f},
            .orientation = Ogre::Quaternion::IDENTITY
        }, 0.5f, mDashboardNode);
        velocityCube.node->setScale(0.01f, 0.01f, 0.01f);

/*
        lightNode = mDashboardNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        lightNode->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.25f));
        light = mSceneMgr->createLight();
        light->setType(Ogre::Light::LT_POINT);
*/
/*
        light->setDiffuseColour(Ogre::ColourValue(0.2f, 0.7f, 1.5f));
        light->setSpecularColour(Ogre::ColourValue(0.5f, 0.8f, 1.5f));
        light->setPowerScale(Ogre::Math::PI * 4.0f);
*/
/*
        constexpr float lightRadius = 0.3f;
        light->setAttenuation(lightRadius, 1.0f, 0.0f, 0.0f);
        lightNode->attachObject(light);
*/
    }

    Dashboard::~Dashboard() {
/*
        mSceneMgr->destroyLight(light);
        mSceneMgr->destroySceneNode(lightNode);
*/
        // mSceneMgr->destroyEntity(velocityCube.entity);
        mSceneMgr->destroySceneNode(velocityCube.node);

        // mSceneMgr->destroyEntity(angularVelocityCube.entity);
        mSceneMgr->destroySceneNode(angularVelocityCube.node);

        // mSceneMgr->destroyEntity(gyroscopeCube.entity);
        mSceneMgr->destroySceneNode(gyroscopeCube.node);
        // mSceneMgr->destroyEntity(deskCube.entity);
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
        auto gyroscopeOrientation = shipOrientation.Inverse();
        gyroscopeCube.node->setOrientation(gyroscopeOrientation);
        Ogre::Vector3 localVelocity(telemetry.localVelocity.x, telemetry.localVelocity.y, telemetry.localVelocity.z);
        float l = localVelocity.length();
        if(l>10.0f)
            localVelocity *= 10.0f / l;
        Ogre::Vector3 vec_(0.015f, 0.0f, 0.015f);
        velocityCube.node->setPosition(vec_ + localVelocity/2000.0f);

        static auto gyroscopeOrientationOld = Ogre::Quaternion::IDENTITY;
        // 1. Получаем дельту в локальных координатах гироскопа
        Ogre::Quaternion deltaQ1_local = gyroscopeOrientationOld.Inverse() * gyroscopeOrientation;
        deltaQ1_local.normalise();

        // 2. Переводим дельту гироскопа в МИРОВЫЕ координаты
        // Для этого сопрягаем её с текущим мировым вращением гироскопа
        Ogre::Quaternion deltaQ1_world = gyroscopeOrientation * deltaQ1_local * gyroscopeOrientation.Inverse();

        // 3. Инвертируем мировую дельту (зеркальное вращение в мировом пространстве)
        Ogre::Quaternion inverseDeltaQ1_world = deltaQ1_world.UnitInverse();

        // 4. Применяем мировую дельту к кубу СЛЕВА (так как приращение мировое)
        auto gyroscopeOrientationNew = inverseDeltaQ1_world * angularVelocityCube.node->getOrientation();
        gyroscopeOrientationNew.normalise();

        angularVelocityCube.node->setOrientation(gyroscopeOrientationNew);
        gyroscopeOrientationOld = gyroscopeOrientation;
    }
}
