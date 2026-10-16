// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameClient_ShipController_H_
#define MaxDoF_GameClient_ShipController_H_

#include <OgreVector3.h>
#include <OgreSceneNode.h>

// Добавляем опережающие объявления Jolt, чтобы не перегружать заголовок
namespace JPH {
    class BodyInterface;
    class BodyID;
}

namespace MaxDoF::GameClient
{
    class ShipController
    {
    private:
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
        ShipController( Ogre::SceneNode *shipNode,
                             JPH::BodyInterface* bodyInterface, JPH::BodyID* shipBodyId );

        bool update( float timeSinceLast, const ControllerData& mControllerData );
    };
}  // namespace MaxDoF::Client

#endif
