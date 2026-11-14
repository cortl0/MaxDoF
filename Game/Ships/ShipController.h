// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <OgreVector3.h>

#include "Common/Ships/Ship.h"
#include "Common/Types/Telemetry.h"

// Добавляем опережающие объявления Jolt, чтобы не перегружать заголовок
namespace JPH {
    class BodyInterface;
    class BodyID;
}

namespace MaxDoF::Game::Ships
{
    class ShipController
    {
        Common::Ships::Ship& mShip;
        JPH::BodyInterface*   mBodyInterface{ nullptr };

        const float           mSensitivityPitch{ 1.0f };
        const float           mSensitivityYaw{ 0.5f };
        const float           mSensitivityRoll{ 0.5f };
        const float           mThrustMultiplier{ 0.5f };

        float                 mThrustPower; // Сила тяги маршевых двигателей в Ньютонах
        float                 ControlPower{10.0f}; // Сила тяги маневровых двигателей в Ньютонах

        bool                  mUseSceneNode;

        JPH::Vec3 mShipPreviousPosition{-JPH::Vec3::sAxisZ()};
        JPH::Quat mShipPreviousOrientation{JPH::Quat::sIdentity()};

    public:
        struct ControllerData
        {
            float mouse_dx{0.0f};
            float mouse_dy{0.0f};
            bool leftMouseButtonPressed{false};
            bool rightMouseButtonPressed{false};
            Ogre::Vector3 moveDir{Ogre::Vector3::ZERO};
        };

        ShipController(Common::Ships::Ship&, JPH::BodyInterface* bodyInterface);
        Common::Types::Telemetry getTelemetry(float timeDelta);
        bool update( float timeSinceLast, const ControllerData& mControllerData );
    };
}
