// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>

namespace MaxDoF::Common::World
{
    struct ContactData
    {
        JPH::BodyID body1;
        JPH::BodyID body2;
        uint64_t userData1;
        uint64_t userData2;

        bool operator<(const ContactData& other) const {
            return std::tie(body1, body2) < std::tie(other.body1, other.body2);
        }

        bool operator==(const ContactData& other) const {
            return body1 == other.body1 && body2 == other.body2;
        }
    };

    class MyContactListener : public JPH::ContactListener
    {
        // TODO: Нужен магический контейнер для производительности.
        // Я придумал кастомный контейнер из нескольких векторов.
        // Этот магический контейнер избаляет от переаллокации памяти и перемещения данных внутри кадра.
        // Когда заканчивается один вектор создаётся следующий, больший по размеру.
        // В конце кадра забираются данные, расчитывается суммарный размер всех векторов,
        // удаляются все векторы и создаётся один вектор суммарного размера.
        // Реализую позже.
        // Провёл тест. Контактов за кадр не более 10. Надо потестировать, когда сцена станет больше.
        // Может быть и нет необходимости в этом магическом контейнере.
        std::vector<ContactData> mContacts;

        std::mutex mMutex;
    public:
        // Вызывается, когда два тела начинают соприкасаться
        virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;

        std::vector<ContactData> FlushContacts();

        // Вызывается каждый кадр, пока тела продолжают соприкасаться
        virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            // Используется для постоянного взаимодействия (например, нанесение урона в лаве)
        }

        // Вызывается, когда тела перестают соприкасаться
        virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
        {
            // Внимание: здесь передаются ID под-форм (SubShapeIDPair), 
            // так как сами тела к этому моменту уже могут быть удалены из контакта.
            // Получить BodyID можно так: inSubShapePair.GetBody1ID()
            //std::cout << "Контакты разорваны между телами." << std::endl;
        }
    };
}
