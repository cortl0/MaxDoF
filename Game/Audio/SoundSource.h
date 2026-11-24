// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <faust/dsp/dsp.h>
#include <faust/gui/MapUI.h>

#include "AudioTypes.h"

namespace MaxDoF::Game::Audio
{
    class SoundSource
    {
    public:
        SoundSource() = default;
        ~SoundSource();

        SoundSource(const SoundSource&) = delete;
        SoundSource& operator=(const SoundSource&) = delete;

        IPLVector3 GetPosition() const { return position; }
        void Init(IPLContext ctx, IPLHRTF hrtf, IPLSimulator simulator, std::unique_ptr<dsp> faustDsp);
        bool IsActive() const { return isActive; }
        void ReadSimulationOutputs(const IPLCoordinateSpace3& listener);
        void RenderAndMix(IPLAudioBuffer& stereoMixOut);
        void SetActive(bool active);
        void SetParam(const std::string& path, float value);
        void SetPosition(IPLVector3 pos) { position = pos; }
        void SetSimulationInputs();

    private:
        float smoothedBlend = 1.0f;
        std::array<DspParamEntry, cMaxSourceParams> m_cachedParams{};
        int numCachedParams = 0;
        IPLVector3 smoothedDir{0.0f, 0.0f, -1.0f};
        float smoothedAtten = 1.0f;
        float smoothedOcc = 1.0f;
        bool hasSmoothedInit = false;
        std::unique_ptr<dsp> mDsp;
        MapUI mMapUI;

        IPLContext mIPLContext = nullptr;
        IPLHRTF mIPLHRTF = nullptr;
        IPLSimulator mIPLSimulator = nullptr;
        IPLSource mIPLSource = nullptr;
        IPLDirectEffect mIPLDirectEffect = nullptr;
        IPLBinauralEffect mIPLBinauralEffect = nullptr;

        IPLVector3 position{0.0f, 0.0f, 0.0f};
        bool isActive = false;

        TripleBuffer<SourceRenderState> stateBuffer;

        std::vector<float> faustMem;
        std::vector<float> directMem;
        std::vector<float> stereoMem;

        float* inPtr = nullptr;
        float* directPtr = nullptr;
        float* stereoPtrs[2]{nullptr, nullptr};

        IPLAudioBuffer inBuf{};
        IPLAudioBuffer directBuf{};
        IPLAudioBuffer stereoBuf{};
    };
}