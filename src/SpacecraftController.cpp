// https://Github/cortl0/MaxDoF
// GPL-3.0 license
#include "SpacecraftController.h"

#include "GraphicsSystem.h"
#include "OgreCamera.h"
#include "OgreSceneNode.h"

// Подключаем необходимые заголовки Jolt для работы с силами
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

namespace MaxDoF
{
    SpacecraftController::SpacecraftController( Demo::GraphicsSystem *graphicsSystem, Ogre::SceneNode *shipNode,
                                                 JPH::BodyInterface* bodyInterface, JPH::BodyID* shipBodyId ) :
        mGraphicsSystem( graphicsSystem ),
        mShipNode( shipNode ),
        mBodyInterface( bodyInterface ),
        mShipBodyId( shipBodyId ),
        mThrustPower( 75000.0f ), // Мощная тяга в Ньютонах для плавного разгона 1500-кг корабля
        mCameraBaseSpeed( 10 ),
        mCameraSpeedBoost( 5 ),
        mUseSceneNode( true )
    {
    }

    bool SpacecraftController::update( float timeSinceLast, const ControllerData& mControllerData )
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

                // Вращаем графику Ogre вокруг локальной оси Z (Roll) и локальной оси X (Pitch)
                mShipNode->roll( Ogre::Radian( mCameraRoll ), Ogre::Node::TS_LOCAL );
                mShipNode->pitch( Ogre::Radian( mCameraPitch ), Ogre::Node::TS_LOCAL );
            }
            else
            {
                // Стандартное поведение (кнопка отпущена): вращаем Yaw (влево-вправо) и Pitch
                mCameraYaw *= mSensitivityYaw;
                mShipNode->yaw( Ogre::Radian( mCameraYaw ), Ogre::Node::TS_LOCAL );
                mShipNode->pitch( Ogre::Radian( mCameraPitch ), Ogre::Node::TS_LOCAL );
            }

            // МГНОВЕННАЯ СИНХРОНИЗАЦИЯ НОВОГО ВРАЩЕНИЯ В JOLT:
            Ogre::Quaternion ogreQuat = mShipNode->getOrientation();
            JPH::Quat joltQuat( ogreQuat.x, ogreQuat.y, ogreQuat.z, ogreQuat.w );
            mBodyInterface->SetRotation( *mShipBodyId, joltQuat, JPH::EActivation::DontActivate );

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
            if( mControllerData.speedBoost )
                currentThrustForce *= mCameraSpeedBoost;

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
}  // namespace MaxDoF
