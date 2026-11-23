// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <array>

#include "AcousticWorld.h"
#include "SoundSource.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <unordered_map>

namespace MaxDoF::Game::Audio
{
    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        void ClearAllSources();
        void CommitScene();
        void DestroySource(SourceId id);
        AcousticWorld& GetAcousticWorld(){ return mAcousticWorld; }
        void SetSourcePosition(SourceId id, IPLVector3 pos);
        void SetSourceParam(SourceId id, const std::string& name, float value);
        void SetSourceActive(SourceId id, bool active);
        SourceId SpawnSource(std::unique_ptr<dsp> faustDsp, IPLVector3 pos, bool autoStart = true);
        bool Start();
        void Stop();
        void Update(IPLVector3 listenerPos, IPLVector3 listenerAhead, IPLVector3 listenerUp);
    private:
        void RenderAudio(float* interleavedOut);
        static void SdlCallback(void* userdata, Uint8* stream, int len);

        static constexpr size_t cFifoCapacity = 8192;
        std::array<float, cFifoCapacity> fifoBuffer{};
        size_t fifoRead = 0;
        size_t fifoWrite = 0;
        size_t fifoCount = 0;

        AcousticWorld mAcousticWorld;
        std::unordered_map<SourceId, std::unique_ptr<SoundSource>> sources;

        SDL_AudioDeviceID mSDL_AudioDeviceID = 0;
        SourceId mSourceId = 1;

        std::vector<float> masterMixMem;
        float* masterMixPtrs[2]{nullptr, nullptr};
        IPLAudioBuffer masterMixBuf{};
    };
}
