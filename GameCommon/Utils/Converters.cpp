// https://Github/cortl0/MaxDoF
// GPL-3.0 license

#include "GameCommon/Utils/Converters.h"

namespace MaxDoF::GameCommon::Utils
{
    JPH::Vec3 getLocalAngularVelocities(const JPH::Quat& qPrev, const JPH::Quat& qCurr, float dt)
    {
        if (dt <= 0.0f)
        return JPH::Vec3::sZero();

        // Локальное изменение
        JPH::Quat qDeltaLocal = qPrev.Inversed() * qCurr;

        // Извлекаем ось и угол вращения
        JPH::Vec3 axis;
        float angle;
        qDeltaLocal.GetAxisAngle(axis, angle); // angle всегда в диапазоне [0, pi]

        // Если угол близок к нулю, вращения почти не было
        if (angle < 1e-6f)
            return JPH::Vec3::sZero();

        // Корректируем направление оси (Jolt возвращает положительный угол, 
        // направление вращения кодируется вектором оси)
        JPH::Vec3 localAngularVelocity = axis * (angle / dt);

        // Возвращаем Vec3, где компоненты X, Y, Z соответствуют локальным осям скорости
        return localAngularVelocity;
    }

    JPH::Vec3 getLocalLinearVelocities(const JPH::Vec3& pPrev, const JPH::Vec3& pCurr, const JPH::Quat& qCurr, float dt)
    {
        // Защита от деления на ноль при микро-апдейтах
        if (dt <= 0.0f)
            return JPH::Vec3::sZero();

        // 1. Вычисляем вектор скорости в мировых координатах (метры в секунду)
        JPH::Vec3 worldVelocity = (pCurr - pPrev) / dt;

        // 2. Переводим мировую скорость в локальные координаты текущей ориентации.
        // Умножение инвертированного кватерниона на мировой вектор смещения
        // трансформирует его в локальное пространство камеры.
        JPH::Vec3 localVelocity = qCurr.Inversed() * worldVelocity;

        return localVelocity;
    }
}  // namespace MaxDoF::GameCommon::Utils
