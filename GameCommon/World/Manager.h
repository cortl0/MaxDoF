// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_World_Manager_H_
#define MaxDoF_World_Manager_H_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameCommon/Types/Entity.h"

#ifdef MAXDOF_CLIENT_BUILD
#include "GameCommon/World/Graphics.h"
#endif

#include "GameCommon/World/Physics.h"
namespace JPH
{
    class TempAllocatorImpl;
    class JobSystemThreadPool;
    class BodyInterface;
}
namespace Ogre { class SceneNode; }

namespace MaxDoF::World
{
    // 1. Заглушка для слоев широкой фазы (BroadPhase)
    class MyBroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
    {
    public:
        virtual JPH::uint GetNumBroadPhaseLayers() const override { return 1; }
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return JPH::BroadPhaseLayer(0); }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override { return "Default"; }
#endif
    };

    // 2. Фильтр: Объект против Слоя широкой фазы (всегда разрешаем коллизию)
    class MyObjectVsBroadPhaseLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override { return true; }
    };

    // 3. Фильтр: Пара Объектов друг с другом (всегда разрешаем коллизию)
    class MyObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override { return true; }
    };

    class Manager
    {
    private:
        JPH::TempAllocatorImpl*     mPhysicsAllocator{ nullptr };
        JPH::JobSystemThreadPool*   mJobSystem{ nullptr };
        JPH::PhysicsSystem*         mPhysicsSystem{ nullptr };

        Physics mPhysics;
#ifdef MAXDOF_CLIENT_BUILD
        MaxDoF::GameClient::World::Graphics mGraphics;
#endif

        std::unique_ptr<MyBroadPhaseLayerInterface>      mBroadPhaseLayerInterface;
        std::unique_ptr<MyObjectVsBroadPhaseLayerFilter> mObjectVsBroadPhaseLayerFilter;
        std::unique_ptr<MyObjectLayerPairFilter>         mObjectLayerPairFilter;

        using Entities = std::vector<Types::Entity>;
        using EntitiesIndex = std::unordered_map<uint32_t, size_t>;

        Entities mEntities;
        EntitiesIndex mEntitiesIndex;

    public:
        Manager();
        ~Manager();

#ifdef MAXDOF_CLIENT_BUILD
        MaxDoF::GameClient::World::Graphics& getGraphics();
#endif
        void initialize(unsigned int maxBodies = 1024);
        Types::Entity createEntity(
#ifdef MAXDOF_CLIENT_BUILD
            Ogre::SceneNode* ogreNode,
#endif
            JPH::BodyID bodyId);
        void update(float timeSinceLast);
        JPH::BodyInterface* getBodyInterface() { return mPhysics.getBodyInterface(); }
        Types::Entity createCube(const Types::PhysPose&, const JPH::RVec3& halfSize, float mass);
#ifdef MAXDOF_CLIENT_BUILD
        Ogre::SceneNode* createLight();
#endif
        Types::Entity createSphere(const Types::PhysPose&, float radius, float mass);
    };
}  // namespace MaxDoF::World

#endif
