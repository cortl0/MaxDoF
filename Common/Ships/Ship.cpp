// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/Ships/Ship.h"

#ifdef MAXDOF_CLIENT_BUILD
#include <OgreItem.h>
#include "Game/Types/Sounds.h"
#endif

namespace MaxDoF::Common::Ships
{
    Ship::Ship(World::Manager& worldManager)
        : mWorldManager(worldManager)
        , mEntity(createShipEntity())
#ifdef MAXDOF_CLIENT_BUILD
        , mDashboard(Game::Ships::Dashboard(mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager(), mEntity.graphNode, Ogre::Vector3(0.0f, -0.1f, -0.3f), mWorldManager.getGraphics()))
#endif
    {
#ifdef MAXDOF_CLIENT_BUILD
        auto* mSceneMgr = mWorldManager.getGraphics().getGraphicsSystem()->getSceneManager();
        lightNode = mEntity.graphNode->createChildSceneNode(Ogre::SCENE_DYNAMIC);
        // lightNode->setPosition(Ogre::Vector3(0.0f, 0.0f, -0.4f));
        light = mSceneMgr->createLight();
        light->setType(Ogre::Light::LT_SPOTLIGHT);
        light->setPowerScale(25.0f);
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

#ifdef MAXDOF_CLIENT_BUILD
    void Ship::update(float timeSinceLastFrame, const Types::Telemetry& telemetry, const Game::Ships::ControllerData& mControllerData )
    {
        mDashboard.update(telemetry);

        if(mWorldManager.PhysicsHavePlace())
        {
            JPH::BodyInterface* bodyInterface = mWorldManager.getBodyInterface();
            // JPH::RVec3 shipPos = bodyInterface->GetPosition(mEntity.physId);
            JPH::Quat shipRot = bodyInterface->GetRotation(mEntity.physId);
            // JPH::Vec3 localForward = -JPH::Vec3::sAxisZ();
            JPH::Vec3 localForward{mControllerData.moveDir.x, mControllerData.moveDir.y, mControllerData.moveDir.z};
            // if(localForward.Length() != 0.0f)
            {
                JPH::Vec3 globalForwardDirection = shipRot * localForward;
                Types::PhysPose projectilePose{/*shipPos +*/ -JPH::RVec3(globalForwardDirection) * 0.5f, shipRot};
                mWorldManager.CreateSound(projectilePose.position, Game::Types::Sounds::cEngine);
            }
        }
    }

#endif
    void Ship::CreateBullet(float timeSinceLast, bool needCreate)
    {
        const float bulletMass{1.0f};
        const float bulletRadius{0.1f};
        static float fireCooldown = 0.0f;
        fireCooldown -= timeSinceLast;

        if(numBullets <= 0)
            return;

        if(!needCreate)
            return;

        if (fireCooldown <= 0.0f)
        {
            --numBullets;
            fireCooldown = 0.1f;

            if(!mWorldManager.PhysicsHavePlace())
                return;

            JPH::BodyInterface* bodyInterface = mWorldManager.getBodyInterface();

            JPH::RVec3 shipPos = bodyInterface->GetPosition(mEntity.physId);
            JPH::Quat shipRot = bodyInterface->GetRotation(mEntity.physId);
            JPH::Vec3 shipVelocity = bodyInterface->GetLinearVelocity(mEntity.physId);

            JPH::Vec3 localForward = -JPH::Vec3::sAxisZ();
            JPH::Vec3 globalForwardDirection = shipRot * localForward;

            Types::PhysPose projectilePose{shipPos + JPH::RVec3(globalForwardDirection) * (0.5f + bulletRadius + 0.1f), shipRot};

            auto& bullet = mWorldManager.createSphere(
                Types::Entity::Type::cBullet,
                projectilePose, bulletRadius, bulletMass, true
            );

            JPH::Vec3 projectileImpulse = shipVelocity + (globalForwardDirection * bulletMass * 100.0f);
            bodyInterface->AddImpulse(bullet.physId, projectileImpulse);

            {
                JPH::BodyLockWrite lock(mWorldManager.getPhysicsSystem()->GetBodyLockInterfaceNoLock(), mEntity.physId);
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
#ifdef MAXDOF_CLIENT_BUILD
            mWorldManager.CreateSound(projectilePose.position, Game::Types::Sounds::cLaunch);
#endif
        }
    }

    Types::Entity& Ship::createShipEntity()
    {
        return mWorldManager.createSphere(
            Types::Entity::Type::cShip,
            {
                .position = JPH::RVec3{0.0, 0.0, 0.0},
                .orientation = JPH::Quat{0.0f, 0.0f, 0.0f, 1.0f}.Normalized()
            }
            , 0.5f, 1000.0f, true
        );
    }
}
