// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/ShipController.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "GameCommon/Utils/Converters.h"
#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient
{
    ShipController::ShipController(const GameCommon::Types::Entity& shipEntity, JPH::BodyInterface* bodyInterface) :
        mShipEntity(shipEntity),
        mBodyInterface(bodyInterface),
        mThrustPower(10000.0f),
        mUseSceneNode(true)
    {
    }

    GameCommon::Types::Telemetry ShipController::ShipController::getTelemetry(float timeDelta)
    {
        JPH::Vec3 position = JPH::Vec3(mBodyInterface->GetPosition(mShipEntity.physId));
        JPH::Quat orientation = mBodyInterface->GetRotation(mShipEntity.physId);
        JPH::Vec3 floatDirection = orientation * -JPH::Vec3::sAxisZ();
        JPH::RVec3 direction(floatDirection);
        JPH::Vec3 angularVelocity = GameCommon::Utils::getLocalAngularVelocities(mShipPreviousOrientation, orientation, timeDelta);
        JPH::Vec3 linearVelocity = GameCommon::Utils::getLocalLinearVelocities(mShipPreviousPosition, position, orientation, timeDelta);
        mShipPreviousPosition = position;
        mShipPreviousOrientation = orientation;
        return GameCommon::Types::Telemetry
        {
            .position = GameCommon::Types::Telemetry::Vector
            {
                .x = position.GetX(),
                .y = position.GetY(),
                .z = position.GetZ()
            },
            .direction = GameCommon::Types::Telemetry::Vector
            {
                .x = static_cast<float>(direction.GetX()),
                .y = static_cast<float>(direction.GetY()),
                .z = static_cast<float>(direction.GetZ())
            },
            .localVelocity = GameCommon::Types::Telemetry::Vector
            {
                .x = linearVelocity.GetX(),
                .y = linearVelocity.GetY(),
                .z = linearVelocity.GetZ()
            },
            .localAngularVelocity = GameCommon::Types::Telemetry::Vector
            {
                .x = angularVelocity.GetX(),
                .y = angularVelocity.GetY(),
                .z = angularVelocity.GetZ()
            }
        };
    }

    bool ShipController::update( float timeSinceLast, const ControllerData& mControllerData )
    {
        if(!mShipEntity.graphNode || !mBodyInterface || mShipEntity.physId.IsInvalid())
            return false;

        bool result{false};

        // ================= 1. ФИЗИЧЕСКОЕ ВРАЩЕНИЕ (МЫШЬ) =================
        float mCameraYaw = -mControllerData.mouse_dx;
        float mCameraPitch = -mControllerData.mouse_dy;

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
                JPH::Quat body_rotation = mBodyInterface->GetRotation(mShipEntity.physId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(mShipEntity.physId, global_torque, JPH::EActivation::Activate);
            }
            else
            {
                // Стандартное поведение (кнопка отпущена): вращаем Yaw (влево-вправо) и Pitch
                mCameraYaw *= mSensitivityYaw;

                const JPH::Vec3 local_torque(mCameraPitch * ControlPower, mCameraYaw * ControlPower, 0.0f); // Момент вокруг оси Y

                // 2. Получаем текущую ориентацию (вращение) тела в мировом пространстве
                JPH::Quat body_rotation = mBodyInterface->GetRotation(mShipEntity.physId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(mShipEntity.physId, global_torque, JPH::EActivation::Activate);
            }

            // МГНОВЕННАЯ СИНХРОНИЗАЦИЯ НОВОГО ВРАЩЕНИЯ В JOLT:
            Ogre::Quaternion ogreQuat = mShipEntity.graphNode->getOrientation();
            JPH::Quat joltQuat( ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w );
            mBodyInterface->SetRotation(mShipEntity.physId, joltQuat, JPH::EActivation::Activate);

            result = true;
        }

        // ================= 2. ФИЗИЧЕСКОЕ ДВИЖЕНИЕ (КЛАВИАТУРА) =================
        if( mControllerData.moveDir != Ogre::Vector3::ZERO )
        {
            Ogre::Vector3 localThrust = mControllerData.moveDir;
            localThrust.normalise();

            // Переводим локальный вектор тяги двигателей (например, W - движение вперед)
            // в мировые координаты на основе текущего вращения корабля в Ogre
            Ogre::Vector3 globalThrustDirection = mShipEntity.graphNode->getOrientation() * localThrust;

            JPH::Vec3 joltForce(
                globalThrustDirection.x * mThrustPower,
                globalThrustDirection.y * mThrustPower,
                globalThrustDirection.z * mThrustPower
            );

            mBodyInterface->AddForce(mShipEntity.physId, joltForce);
            result = true;
        }

        return result;
    }
}
