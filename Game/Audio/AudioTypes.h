// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <phonon.h>
#include <array>
#include <atomic>
#include <cstdint>

namespace MaxDoF::Game::Audio
{
    constexpr int cAudioSampleRate = 48000;
    constexpr int cAudioFrameSize = 512;

    using SourceId = uint32_t;
    constexpr SourceId cInvalidSourceId = 0;

    namespace AcousticMaterials
    {
        constexpr IPLMaterial cBrick      { {0.03f, 0.04f, 0.07f}, 0.050f, {0.025f, 0.005f, 0.002f} };
        constexpr IPLMaterial cConcrete   { {0.05f, 0.07f, 0.08f}, 0.015f, {0.015f, 0.002f, 0.001f} };
        constexpr IPLMaterial cHeavyHull  { {0.10f, 0.10f, 0.10f}, 0.010f, {0.0005f, 0.0001f, 0.0000f} };
        constexpr IPLMaterial cGlass      { {0.18f, 0.06f, 0.04f}, 0.010f, {0.400f, 0.250f, 0.150f} };
        constexpr IPLMaterial cSoundproof { {0.50f, 0.50f, 0.50f}, 0.010f, {0.0000f, 0.0000f, 0.0000f} };
        constexpr IPLMaterial cWood       { {0.11f, 0.08f, 0.06f}, 0.050f, {0.200f, 0.080f, 0.030f} };
    }

    struct DspParamEntry
    {
        int paramIndex = -1;
        float value = 0.0f;
    };

    constexpr int cMaxSourceParams = 16;

    struct SourceRenderState
    {
        IPLDirectEffectParams directParams{};
        IPLVector3 direction{0.0f, 0.0f, -1.0f};
        float spatialBlend = 1.0f;
        bool active = false;
        std::array<DspParamEntry, cMaxSourceParams> params{};
        int numParams = 0;
    };

    template <typename T>
    class TripleBuffer
    {
    public:
        const T& Read()
        {
            int latest = latestIdx.load(std::memory_order_acquire);
            readIdx.store(latest, std::memory_order_release);
            return buffers.at(latest);
        }

        void Write(const T& data)
        {
            int nextWrite = (writeIdx + 1) % 3;
            if (nextWrite == readIdx.load(std::memory_order_acquire))
                nextWrite = (nextWrite + 1) % 3;
            buffers.at(nextWrite) = data;
            writeIdx = nextWrite;
            latestIdx.store(nextWrite, std::memory_order_release);
        }

    private:
        std::array<T, 3> buffers{};
        std::atomic<int> latestIdx{0};
        std::atomic<int> readIdx{0};
        int writeIdx = 0;
    };
}
