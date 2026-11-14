// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Ships/Ship.h"

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreItem.h>
#endif

namespace MaxDoF::Common::Ships
{
    Ship::Ship(
        World::Manager& worldManager,
#ifdef MAXDOF_CLIENT_BUILD
        Game::World::Graphics& graphics,
#endif
        const Types::Entity& entity)
        : mWorldManager(worldManager)
#ifdef MAXDOF_CLIENT_BUILD
        , mDashboard(mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager(), entity.graphNode, Ogre::Vector3(0.0f, -0.1f, -0.3f), graphics)
#endif
        , mEntity(entity)
    {
#ifdef MAXDOF_CLIENT_BUILD
        auto* mSceneMgr = mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager();
        lightNode = mEntity.graphNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        // lightNode->setPosition(Ogre::Vector3(0.0f, 0.0f, -0.4f));
        light = mSceneMgr->createLight();
        light->setType(Ogre::Light::LT_SPOTLIGHT);
        light->setPowerScale(5.0f);
        constexpr float lightRadius = 500.0f;
        light->setAttenuation(lightRadius, 1.0f, 0.0f, 0.01f);
        light->setSpotlightRange(Ogre::Degree(0), Ogre::Degree(60));
        lightNode->attachObject(light);
#endif
    }

    Ship::~Ship()
    {
#ifdef MAXDOF_CLIENT_BUILD
        auto* sceneManager = mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager();
        sceneManager->destroyLight(light);
        sceneManager->destroySceneNode(lightNode);
#endif
    }

#ifdef MAXDOF_CLIENT_BUILD
    void Ship::connectCameraToShip(Ogre::Camera* camera)
    {
        camera->setFarClipDistance(10000.0f);
        if(camera->getParentSceneNode())
            camera->getParentSceneNode()->detachObject(camera);
        mEntity.graphNode->attachObject(camera);
        camera->setPosition(Ogre::Vector3::ZERO);
        //camera->setPosition(Ogre::Vector3(0.0f, 0.0f, 10.0f));
        camera->setOrientation(Ogre::Quaternion::IDENTITY);
    }
#endif

    void Ship::update(float timeSinceLastFrame, const Types::Telemetry& telemetry)
    {
#ifdef MAXDOF_CLIENT_BUILD
        mDashboard.update(telemetry);
#endif
    }

    void Ship::CreateBullet(float timeSinceLast, bool needCreate)
    {
        const float bulletMass{1.0f};
        static float fireCooldown = 0.0f;
        fireCooldown -= timeSinceLast;

        if(!needCreate)
        {
            if (fireCooldown <= 0.0f)
                fireCooldown = 0.0f;

            return;
        }

        if (fireCooldown <= 0.0f)
        {
            fireCooldown = 0.1f;

            JPH::BodyInterface* bodyInterface = mWorldManager.getBodyInterface();

            JPH::RVec3 shipPos = bodyInterface->GetPosition(mEntity.physId);
            JPH::Quat shipRot = bodyInterface->GetRotation(mEntity.physId);
            JPH::Vec3 shipVelocity = bodyInterface->GetLinearVelocity(mEntity.physId);

            JPH::Vec3 localForward = -JPH::Vec3::sAxisZ();
            JPH::Vec3 globalForwardDirection = shipRot * localForward;

            Types::PhysPose projectilePose{shipPos + JPH::RVec3(globalForwardDirection) * 1.5f, shipRot};

            auto& bullet = mWorldManager.createSphere(Types3d::EntityType::cBullet, projectilePose, 0.1f, bulletMass, true);
            JPH::Vec3 projectileImpulse = shipVelocity + (globalForwardDirection * bulletMass * 100.0f);
            bodyInterface->AddImpulse(bullet.physId, projectileImpulse);

            {
                JPH::BodyLockWrite lock(mWorldManager.getPhysicsSystem()->GetBodyLockInterface(), mEntity.physId);
                if (lock.Succeeded())
                {
                    JPH::Body &body = lock.GetBody();

                    if (body.IsDynamic())
                    {
                        JPH::MotionProperties* motionProperties = body.GetMotionProperties();
                        auto mass = motionProperties->GetInverseMass();
                        mass = 1.0f / mass;
                        mass -= bulletMass;
                        mass = 1.0f / mass;
                        motionProperties->ScaleToMass(1.0f / mass);
                    }
                }
            }

            bodyInterface->AddImpulse(mEntity.physId, -projectileImpulse);
        }
    }
}
