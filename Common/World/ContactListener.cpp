// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Common/World/ContactListener.h"

#include <iostream>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#ifdef MAXDOF_CLIENT_BUILD
#include "Game/Utils/Converters.h"
#include "ContactListener.h"
#endif

namespace MaxDoF::Common::World
{
    void MyContactListener::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
    {
        JPH::Vec3 localAvg = JPH::Vec3::sZero();
        const auto& points = inManifold.mRelativeContactPointsOn1;

        if (!points.empty())
        {
            for (const JPH::Vec3& p : points)
                localAvg += p;

            localAvg /= static_cast<float>(points.size());
        }

        JPH::RVec3 worldPoint = inManifold.mBaseOffset + localAvg;

        std::lock_guard<std::mutex> lock(mMutex);

        mContacts.push_back({
            inBody1.GetID(),
            inBody2.GetID(),
            inBody1.GetUserData(),
            inBody2.GetUserData(),
            worldPoint
        });
    }

    void optimize(std::vector<ContactData>& data)
    {
        for (auto& contact : data) {
            if (contact.body1.GetIndex() > contact.body2.GetIndex()) {
                std::swap(contact.body1, contact.body2);
                std::swap(contact.userData1, contact.userData2);
            }
        }

        std::sort(data.begin(), data.end());
        data.erase(std::unique(data.begin(), data.end()), data.end());
    }

    std::vector<ContactData> MyContactListener::FlushContacts()
    {
        std::vector<ContactData> output;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            output.swap(mContacts);
        }

        // TODO: Когда сцена вырастет надо понять нужна ли эта оптимизация.
        optimize(output);

        return output;
    }
}
