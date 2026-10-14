// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_SpacecraftController_H_
#define MaxDoF_SpacecraftController_H_

#include "GraphicsSystem.h"
#include "OgrePrerequisites.h"
#include "OgreVector3.h"

// Добавляем опережающие объявления Jolt, чтобы не перегружать заголовок
namespace JPH {
    class BodyInterface;
    class BodyID;
}

namespace MaxDoF
{
    class SpacecraftController
    {
    private:
        Demo::GraphicsSystem* mGraphicsSystem{ nullptr };
        Ogre::SceneNode*      mShipNode{ nullptr };

        // ИСПРАВЛЕНИЕ: Добавляем хранение физических интерфейсов корабля
        JPH::BodyInterface*   mBodyInterface{ nullptr };
        JPH::BodyID*          mShipBodyId{ nullptr };

        const float           mSensitivityPitch{ -0.0010f };
        const float           mSensitivityYaw{ -0.0005f };
        const float           mSensitivityRoll{ -0.0012f };

        const float           mThrustMultiplier{ 0.5f };

        float                 mThrustPower; // Сила тяги маршевых двигателей в Ньютонах

        float                 mCameraBaseSpeed;
        float                 mCameraSpeedBoost;
        bool                  mUseSceneNode;

    public:
        struct ControllerData
        {
            float mouse_dx{0.0f};
            float mouse_dy{0.0f};
            bool rightMouseButtonPressed{false};
            Ogre::Vector3 moveDir{Ogre::Vector3::ZERO};
            bool speedBoost{false};
        };

        // Обновленный конструктор принимает узел графики, интерфейс физики и ID тела корабля
        SpacecraftController( Demo::GraphicsSystem *graphicsSystem, Ogre::SceneNode *shipNode,
                             JPH::BodyInterface* bodyInterface, JPH::BodyID* shipBodyId );

        bool update( float timeSinceLast, const ControllerData& mControllerData );
    };
}  // namespace MaxDoF

#endif
