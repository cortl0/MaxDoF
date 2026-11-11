// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Game/ShipController.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "Common/Utils/Converters.h"
#include "Common/Types/Telemetry.h"

namespace MaxDoF::Game
{
    ShipController::ShipController(Game::Ships::Ship& ship, JPH::BodyInterface* bodyInterface) :
        mShip(ship),
        mBodyInterface(bodyInterface),
        mThrustPower(10000.0f),
        mUseSceneNode(true)
    {
    }

    Common::Types::Telemetry ShipController::ShipController::getTelemetry(float timeDelta)
    {
        const auto& shipEntity = mShip.getEntity();
        JPH::Vec3 position = JPH::Vec3(mBodyInterface->GetPosition(shipEntity.physId));
        JPH::Quat orientation = mBodyInterface->GetRotation(shipEntity.physId);
        JPH::Vec3 floatDirection = orientation * -JPH::Vec3::sAxisZ();
        JPH::RVec3 direction(floatDirection);
        JPH::Vec3 angularVelocity = Common::Utils::getLocalAngularVelocities(mShipPreviousOrientation, orientation, timeDelta);
        JPH::Vec3 linearVelocity = Common::Utils::getLocalLinearVelocities(mShipPreviousPosition, position, orientation, timeDelta);
        mShipPreviousPosition = position;
        mShipPreviousOrientation = orientation;
        return Common::Types::Telemetry
        {
            .position = Common::Types::Telemetry::Vector
            {
                .x = position.GetX(),
                .y = position.GetY(),
                .z = position.GetZ()
            },
            .direction = Common::Types::Telemetry::Vector
            {
                .x = static_cast<float>(direction.GetX()),
                .y = static_cast<float>(direction.GetY()),
                .z = static_cast<float>(direction.GetZ())
            },
            .localVelocity = Common::Types::Telemetry::Vector
            {
                .x = linearVelocity.GetX(),
                .y = linearVelocity.GetY(),
                .z = linearVelocity.GetZ()
            },
            .localAngularVelocity = Common::Types::Telemetry::Vector
            {
                .x = angularVelocity.GetX(),
                .y = angularVelocity.GetY(),
                .z = angularVelocity.GetZ()
            }
        };
    }

    bool ShipController::update( float timeSinceLast, const ControllerData& mControllerData )
    {
        const auto& shipEntity = mShip.getEntity();
        if(!shipEntity.graphNode || !mBodyInterface || shipEntity.physId.IsInvalid())
            return false;

        bool result{false};

        // ================= 1. ФИЗИЧЕСКОЕ ВРАЩЕНИЕ (МЫШЬ) =================
        float mCameraYaw = -mControllerData.mouse_dx;
        float mCameraPitch = -mControllerData.mouse_dy;
        mShip.CreateBullet(timeSinceLast, mControllerData.leftMouseButtonPressed);

        if( mCameraYaw != 0.0f || mCameraPitch != 0.0f )
        {
            mCameraPitch *= mSensitivityPitch; // Pitch (вверх-вниз) работает всегда одинаково

            // Проверяем: если зажата ПРАВАЯ кнопка мыши — делаем КРЕН (Roll) вместо рыскания
            if( mControllerData.rightMouseButtonPressed )
            {
                // Переводим горизонтальное движение мыши в угол крена (Roll)
                float mCameraRoll = mCameraYaw * mSensitivityRoll;
                const JPH::Vec3 local_torque(0.0f, 0.0f, mCameraRoll * ControlPower); // Момент вокруг оси Y

                // 2. Получаем текущую ориентацию (вращение) тела в мировом пространстве
                JPH::Quat body_rotation = mBodyInterface->GetRotation(shipEntity.physId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(shipEntity.physId, global_torque, JPH::EActivation::Activate);
            }
            else
            {
                // Стандартное поведение (кнопка отпущена): вращаем Yaw (влево-вправо) и Pitch
                mCameraYaw *= mSensitivityYaw;

                const JPH::Vec3 local_torque(mCameraPitch * ControlPower, mCameraYaw * ControlPower, 0.0f); // Момент вокруг оси Y

                // 2. Получаем текущую ориентацию (вращение) тела в мировом пространстве
                JPH::Quat body_rotation = mBodyInterface->GetRotation(shipEntity.physId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(shipEntity.physId, global_torque, JPH::EActivation::Activate);
            }

            // МГНОВЕННАЯ СИНХРОНИЗАЦИЯ НОВОГО ВРАЩЕНИЯ В JOLT:
            Ogre::Quaternion ogreQuat = shipEntity.graphNode->getOrientation();
            JPH::Quat joltQuat( ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w );
            mBodyInterface->SetRotation(shipEntity.physId, joltQuat, JPH::EActivation::Activate);

            result = true;
        }

        // ================= 2. ФИЗИЧЕСКОЕ ДВИЖЕНИЕ (КЛАВИАТУРА) =================
        if( mControllerData.moveDir != Ogre::Vector3::ZERO )
        {
            Ogre::Vector3 localThrust = mControllerData.moveDir;
            localThrust.normalise();

            // Переводим локальный вектор тяги двигателей (например, W - движение вперед)
            // в мировые координаты на основе текущего вращения корабля в Ogre
            Ogre::Vector3 globalThrustDirection = shipEntity.graphNode->getOrientation() * localThrust;

            JPH::Vec3 joltForce(
                globalThrustDirection.x * mThrustPower,
                globalThrustDirection.y * mThrustPower,
                globalThrustDirection.z * mThrustPower
            );

            mBodyInterface->AddForce(shipEntity.physId, joltForce);
            result = true;
        }

        return result;
    }
}
