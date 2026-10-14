// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#ifndef MaxDoF_World_PhysicsSystemManager_H_
#define MaxDoF_World_PhysicsSystemManager_H_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>

// Обязательные интерфейсы фильтрации для Jolt 5.x+
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <Utils/Types.h>

#include "Utils/Types.h"
#include "World/Entity.h"
#include "World/Physics.h"
#include "World/Graphics.h"
#include "Graphics.h"

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

    class PhysicsSystemManager
    {
    private:
        JPH::TempAllocatorImpl*     mPhysicsAllocator{ nullptr };
        JPH::JobSystemThreadPool*   mJobSystem{ nullptr };
        JPH::PhysicsSystem*         mPhysicsSystem{ nullptr };
        Physics mPhysics;

        std::unique_ptr<MyBroadPhaseLayerInterface>      mBroadPhaseLayerInterface;
        std::unique_ptr<MyObjectVsBroadPhaseLayerFilter> mObjectVsBroadPhaseLayerFilter;
        std::unique_ptr<MyObjectLayerPairFilter>         mObjectLayerPairFilter;

        using Entities = std::vector<World::Entity>;
        using EntitiesIndex = std::unordered_map<uint32_t, size_t>;

        Entities mEntities;
        EntitiesIndex mEntitiesIndex;

    public:
        Graphics mGraphics;
        PhysicsSystemManager();
        ~PhysicsSystemManager();

        void initialize(unsigned int maxBodies = 1024);
        World::Entity createEntity(Ogre::SceneNode* ogreNode, JPH::BodyID bodyId);
        void update(float timeSinceLast);
        //void syncPhysicsToGraphics(JPH::BodyID bodyId, Ogre::SceneNode* ogreNode);
        JPH::BodyInterface* getBodyInterface() { return mPhysics.getBodyInterface(); }

        World::Entity createCube(const Types::PhysPose&, const JPH::RVec3& halfSize, float mass);
        Ogre::SceneNode* createLight();
        World::Entity createSphere(const Types::PhysPose&, float radius, float mass);
    };
}  // namespace MaxDoF::World

#endif
