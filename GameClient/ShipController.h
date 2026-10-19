// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#ifndef MaxDoF_GameClient_ShipController_H_
#define MaxDoF_GameClient_ShipController_H_

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <OgreVector3.h>
#include <OgreSceneNode.h>

#include "GameCommon/Types/Telemetry.h"

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

        const float           mSensitivityPitch{ -1.1f };
        const float           mSensitivityYaw{ -0.5f };
        const float           mSensitivityRoll{ -1.2f };
        const float           mThrustMultiplier{ 0.5f };

        float                 mThrustPower; // Сила тяги маршевых двигателей в Ньютонах
        float                 ControlPower{10.0f}; // Сила тяги маневровых двигателей в Ньютонах

        float                 mCameraBaseSpeed;
        bool                  mUseSceneNode;

        JPH::Vec3 mShipPreviousPosition{-JPH::Vec3::sAxisZ()};
        JPH::Quat mShipPreviousOrientation{JPH::Quat::sIdentity()};

    public:
        struct ControllerData
        {
            float mouse_dx{0.0f};
            float mouse_dy{0.0f};
            bool rightMouseButtonPressed{false};
            Ogre::Vector3 moveDir{Ogre::Vector3::ZERO};
        };

        // Обновленный конструктор принимает узел графики, интерфейс физики и ID тела корабля
        ShipController( Ogre::SceneNode *shipNode,
                             JPH::BodyInterface* bodyInterface, JPH::BodyID* shipBodyId );
        GameCommon::Types::Telemetry getTelemetry(float timeDelta);
        bool update( float timeSinceLast, const ControllerData& mControllerData );
    };
}  // namespace MaxDoF::GameClient

#endif
