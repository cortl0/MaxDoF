// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/Manager.h"

#ifdef MAXDOF_CLIENT_BUILD
#include <faust/gui/meta.h>
#include "SineTest.h"
#include "Game/Utils/Converters.h"
#endif

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Common::World
{
    Manager::Manager()
#ifdef MAXDOF_CLIENT_BUILD
        : mGraphics("")
#endif
    {
    }

    Manager::~Manager()
    {
    }

    void Manager::initialize(unsigned int maxBodies)
    {
        mPhysics.initialize();
        mPhysics.setBodyInterface(&mPhysics.getPhysicsSystem()->GetBodyInterface());
        mPhysics.getPhysicsSystem()->SetGravity(JPH::Vec3::sZero());
        mEntities.Reserve(maxBodies);

#ifdef MAXDOF_CLIENT_BUILD
        mGraphics.initialize();
        initAudio();
#endif
    }

    void Manager::update(float timeSinceLast, const JPH::BodyID& shipBodyId)
    {
        if (!mPhysics.getPhysicsSystem())
            return;

        float physicsTimeStep = std::min(timeSinceLast, 0.1f);
        mPhysics.update(physicsTimeStep);

#ifdef MAXDOF_CLIENT_BUILD
        auto shipPosition = mPhysics.getBodyInterface()->GetPosition(shipBodyId);
        auto shipOrientation = mPhysics.getBodyInterface()->GetRotation(shipBodyId);
        for (const auto& entity : mEntities.GetEntities())
        {
            if (!entity.graphNode)
                continue;

            JPH::RVec3 position;
            JPH::Quat rotation;
            mPhysics.getBodyInterface()->GetPositionAndRotation(entity.physId, position, rotation);
            JPH::Vec3 localPos = JPH::Vec3(position - shipPosition);
            entity.graphNode->setPosition(localPos.GetX(), localPos.GetY(), localPos.GetZ());
            entity.graphNode->setOrientation(Ogre::Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()));
        }
        mGraphics.setPositionOfGlobalStaticNode(Game::Utils::JphRVec3ToOgreVector3(-shipPosition));

        if (!m_staticAudioMeshIds.empty())
        {
            IPLMatrix4x4 t = m_audio.GetAcousticWorld().MakeIPLTransform(
                -static_cast<float>(shipPosition.GetX()),
                -static_cast<float>(shipPosition.GetY()),
                -static_cast<float>(shipPosition.GetZ()),
                0.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f
            );

            for (int id : m_staticAudioMeshIds)
                m_audio.GetAcousticWorld().UpdateDynamicMeshTransform(id, t);

            m_audio.CommitScene();
        }

        if (m_ambientSource != Game::Audio::cInvalidSourceId)
        {
            m_audio.SetSourcePosition(m_ambientSource, {
                static_cast<float>(-shipPosition.GetX()),
                static_cast<float>(-shipPosition.GetY()),
                static_cast<float>(-shipPosition.GetZ())
            });
        }

        auto forward = shipOrientation * -JPH::Vec3::sAxisZ();
        auto up = shipOrientation * JPH::Vec3::sAxisY();
        m_audio.Update(
            {0.0f, 0.0f, 0.0f},
            {forward.GetX(), forward.GetY(), forward.GetZ()},
            {up.GetX(), up.GetY(), up.GetZ()}
        );
#endif
    }

    Types::Entity& Manager::createEntity(
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* ogreNode,
#endif
        JPH::BodyID bodyId)
    {
        mEntities.AddEntity(bodyId
#ifdef MAXDOF_CLIENT_BUILD
            , ogreNode
#endif
        );

        return mEntities.GetEntities().back();
    }

    void Manager::destroyEntity(JPH::uint32 bodyIndex)
    {
        auto entity = mEntities.RemoveEntity(bodyIndex);

        if(!entity.has_value())
            return;

        mPhysics.destroyBody(entity.value().physId);

#ifdef MAXDOF_CLIENT_BUILD
        mGraphics.destroyNodeRecursively(entity.value().graphNode);

        // TODO: Когда код будет готов и я буду уверен, что этот объект обладает звуком надо будет удалить звук
        if(0)
        if (m_ambientSource != Game::Audio::cInvalidSourceId)
        {
            m_audio.DestroySource(m_ambientSource);
            m_ambientSource = Game::Audio::cInvalidSourceId;
        }
#endif
    }

    Types::Entity& Manager::createCube(Common::Types3d::EntityType userData, const Types::PhysPose& pose, const JPH::Vec3& halfSize, float mass, bool dynamic, bool onCCD)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createCube(MaxDoF::Game::Utils::PhysPoseToGraphPose(pose), MaxDoF::Game::Utils::JphVec3ToOgreVector3(halfSize)),
#endif
            mPhysics.createBox(userData, pose, halfSize, mass, dynamic, onCCD));
    }

    Types::Entity& Manager::createMesh(const Common::Types3d::MeshData& meshData)
    {
        if (meshData.vertices.empty() || meshData.indices.empty())
        {
            MaxDoF_LOG_AND_ABORT("bad data")
        }

#ifdef MAXDOF_CLIENT_BUILD
        addMeshToAudio(meshData);
#endif

        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createMesh(meshData),
#endif
            mPhysics.createMesh(meshData));
    }

    Types::Entity& Manager::createSphere(Common::Types3d::EntityType userData, const Types::PhysPose& pose, float radius, float mass, bool onCCD)
    {
        return createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            mGraphics.createSphere( MaxDoF::Game::Utils::PhysPoseToGraphPose( pose ), radius ),
#endif
            mPhysics.createSphere(userData, pose, radius, mass, onCCD));
    }

#ifdef MAXDOF_CLIENT_BUILD
    void Manager::addMeshToAudio(const Common::Types3d::MeshData& meshData)
    {
        std::vector<IPLVector3> wallVerts;
        wallVerts.reserve(meshData.vertices.size());
        std::vector<IPLTriangle> wallTris;
        wallTris.reserve(meshData.indices.size() / 3);

        for (const auto& v : meshData.vertices)
            wallVerts.push_back(IPLVector3{.x = static_cast<float>(v.x), .y = static_cast<float>(v.y), .z = static_cast<float>(v.z)});

        if (meshData.indices.size() % 3 != 0)
            MaxDoF_LOG_AND_ABORT("bad data")

        for (size_t i = 0; i + 2 < meshData.indices.size(); i += 3)
        {
            if (i + 2 < meshData.indices.size())
            {
                IPLTriangle ite{};
                ite.indices[0] = static_cast<IPLint32>(meshData.indices[i]);
                ite.indices[1] = static_cast<IPLint32>(meshData.indices[i + 1]);
                ite.indices[2] = static_cast<IPLint32>(meshData.indices[i + 2]);
                wallTris.push_back(ite);
            }
        }

         IPLMatrix4x4 t = m_audio.GetAcousticWorld().MakeIPLTransform(
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f
        );

        int meshId = m_audio.GetAcousticWorld().AddDynamicMesh(wallVerts, wallTris, Game::Audio::AcousticMaterials::cHeavyHull, t);
        m_staticAudioMeshIds.push_back(meshId);
        m_audio.CommitScene();
    }

    void Manager::initAudio()
    {
        m_audio.CommitScene();

        m_ambientSource = m_audio.SpawnSource(std::make_unique<SineTest>(), {0.0f, 0.0f, 0.0f});
        m_audio.SetSourceParam(m_ambientSource, "freq", 1000.0f);
        m_audio.SetSourceParam(m_ambientSource, "gain", 0.25f);

        m_audio.Update({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
        m_audio.Start();
    }
#endif
}
