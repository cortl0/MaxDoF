// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameClient/ShipController.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "GameCommon/Utils/Converters.h"
#include "GameCommon/Types/Telemetry.h"

namespace MaxDoF::GameClient
{
    ShipController::ShipController( Ogre::SceneNode *shipNode,
                                                 JPH::BodyInterface* bodyInterface, JPH::BodyID* shipBodyId ) :
        mShipNode( shipNode ),
        mBodyInterface( bodyInterface ),
        mShipBodyId( shipBodyId ),
        mThrustPower( 10000.0f ),
        mCameraBaseSpeed( 10 ),
        mUseSceneNode( true )
    {
    }

    GameCommon::Types::Telemetry ShipController::ShipController::getTelemetry(float timeDelta)
    {
        JPH::RVec3 position = mBodyInterface->GetPosition(*mShipBodyId);
        JPH::Quat orientation = mBodyInterface->GetRotation(*mShipBodyId);
        JPH::RVec3 direction = orientation * -JPH::Vec3::sAxisZ();
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
                .x = direction.GetX(),
                .y = direction.GetY(),
                .z = direction.GetZ()
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
        if( !mShipNode || !mBodyInterface || !mShipBodyId )
            return false;

        bool result{false};

        // ================= 1. ФИЗИЧЕСКОЕ ВРАЩЕНИЕ (МЫШЬ) =================
        float mCameraYaw = mControllerData.mouse_dx;
        float mCameraPitch = mControllerData.mouse_dy;

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
                JPH::Quat body_rotation = mBodyInterface->GetRotation(*mShipBodyId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(*mShipBodyId, global_torque, JPH::EActivation::Activate);
            }
            else
            {
                // Стандартное поведение (кнопка отпущена): вращаем Yaw (влево-вправо) и Pitch
                mCameraYaw *= mSensitivityYaw;

                const JPH::Vec3 local_torque(mCameraPitch * ControlPower, mCameraYaw * ControlPower, 0.0f); // Момент вокруг оси Y

                // 2. Получаем текущую ориентацию (вращение) тела в мировом пространстве
                JPH::Quat body_rotation = mBodyInterface->GetRotation(*mShipBodyId);

                // 3. Переводим локальный вектор в глобальный (умножаем кватернион на вектор)
                JPH::Vec3 global_torque = body_rotation * local_torque;

                // Передаем момент по BodyID. Третий параметр автоматически будит тело, если оно спало.
                mBodyInterface->AddTorque(*mShipBodyId, global_torque, JPH::EActivation::Activate);
            }

            // МГНОВЕННАЯ СИНХРОНИЗАЦИЯ НОВОГО ВРАЩЕНИЯ В JOLT:
            Ogre::Quaternion ogreQuat = mShipNode->getOrientation();
            JPH::Quat joltQuat( ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w );
            mBodyInterface->SetRotation( *mShipBodyId, joltQuat, JPH::EActivation::Activate );

            result = true;
        }

        // ================= 2. ФИЗИЧЕСКОЕ ДВИЖЕНИЕ (КЛАВИАТУРА) =================
        if( mControllerData.moveDir != Ogre::Vector3::ZERO )
        {
            Ogre::Vector3 localThrust = mControllerData.moveDir;
            localThrust.normalise();

            // Переводим локальный вектор тяги двигателей (например, W - движение вперед)
            // в мировые координаты на основе текущего вращения корабля в Ogre
            Ogre::Vector3 globalThrustDirection = mShipNode->getOrientation() * localThrust;

            // Считаем итоговую силу с учетом зажатого Shift (Boost)
            float currentThrustForce = mThrustPower * mThrustMultiplier;

            // Формируем вектор силы для Jolt Physics
            JPH::Vec3 joltForce(
                globalThrustDirection.x * currentThrustForce,
                globalThrustDirection.y * currentThrustForce,
                globalThrustDirection.z * currentThrustForce
            );

            // Прикладываем силу к центру масс физического тела корабля!
            // Движок Jolt сам рассчитает ускорение a = F / m и изменит скорость на этом кадре
            mBodyInterface->AddForce( *mShipBodyId, joltForce );

            result = true;
        }

        return result;
    }
}  // namespace MaxDoF::GameClient
