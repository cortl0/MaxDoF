// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/World/Entities.h"

#ifdef MAXDOF_CLIENT_BUILD
#include "Game/World/Graphics.h"
#endif

#include "Common/World/Physics.h"

namespace Ogre { class SceneNode; }

namespace MaxDoF::Common::World
{
    class Manager
    {
        World::Entites mEntities;
        Physics mPhysics;

#ifdef MAXDOF_CLIENT_BUILD
        MaxDoF::Game::World::Graphics mGraphics;
#endif

    public:
        Manager();
        ~Manager();

#ifdef MAXDOF_CLIENT_BUILD
        MaxDoF::Game::World::Graphics& getGraphics(){ return mGraphics; }
#endif
        JPH::PhysicsSystem* getPhysicsSystem(){ return mPhysics.getPhysicsSystem(); }
        void initialize(unsigned int maxBodies);

        Types::Entity& createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            Ogre::SceneNode* ogreNode,
#endif
            JPH::BodyID bodyId);

        void destroyEntity(JPH::uint32 bodyIndex);
        void update(float timeSinceLast, const JPH::BodyID& shipBodyId);
        JPH::BodyInterface* getBodyInterface() { return mPhysics.getBodyInterface(); }
        Types::Entity& createCube(JPH::uint64 userData, const Types::PhysPose&, const JPH::Vec3& halfSize, float mass, bool dynamic = true, bool onCCD = false);
        Types::Entity& createMesh(const Common::Types3d::MeshData& meshData);
        Types::Entity& createSphere(JPH::uint64 userData, const Types::PhysPose&, float radius, float mass, bool onCCD = false);
    };
}
