// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "AudioEngine.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace MaxDoF::Game::Audio
{
    AudioEngine::AudioEngine()
    {
        masterMixMem.resize(cAudioFrameSize * 2, 0.0f);
        masterMixPtrs[0] = masterMixMem.data();
        masterMixPtrs[1] = masterMixMem.data() + cAudioFrameSize;
        masterMixBuf = { 2, cAudioFrameSize, masterMixPtrs };
    }

    AudioEngine::~AudioEngine()
    {
        Stop();
    }

    void AudioEngine::ClearAllSources()
    {
        if (mSDL_AudioDeviceID != 0)
            SDL_LockAudioDevice(mSDL_AudioDeviceID);

        sources.clear();

        if (mSDL_AudioDeviceID != 0)
            SDL_UnlockAudioDevice(mSDL_AudioDeviceID);

        iplSimulatorCommit(mAcousticWorld.GetSimulator());
    }

    void AudioEngine::CommitScene()
    {
        mAcousticWorld.CommitScene();
    }

    void AudioEngine::DestroySource(SourceId id)
    {
        if (id == cInvalidSourceId)
            return;

        auto it = sources.find(id);
        if (it == sources.end())
            return;

        if (mSDL_AudioDeviceID != 0)
            SDL_LockAudioDevice(mSDL_AudioDeviceID);

        sources.erase(it);

        if (mSDL_AudioDeviceID != 0)
            SDL_UnlockAudioDevice(mSDL_AudioDeviceID);

        iplSimulatorCommit(mAcousticWorld.GetSimulator());
    }

    void AudioEngine::RenderAudio(float* interleavedOut)
    {
        std::fill(masterMixMem.begin(), masterMixMem.end(), 0.0f);

        for (auto& src : sources)
            src.second->RenderAndMix(masterMixBuf);

        for (float& sample : masterMixMem)
            sample = std::tanh(sample);

        iplAudioBufferInterleave(mAcousticWorld.GetContext(), &masterMixBuf, interleavedOut);
    }

    void AudioEngine::SdlCallback(void* userdata, Uint8* stream, int len)
    {
        auto* engine = static_cast<AudioEngine*>(userdata);
        size_t floatsNeeded = static_cast<size_t>(len) / sizeof(float);
        float* out = reinterpret_cast<float*>(stream);

        constexpr size_t blockFloats = cAudioFrameSize * 2;

        if (floatsNeeded == blockFloats && engine->fifoCount == 0)
        {
            engine->RenderAudio(out);
            return;
        }

        while (engine->fifoCount < floatsNeeded)
        {
            if (engine->fifoCount + blockFloats > cFifoCapacity)
                break;

            float tempBlock[blockFloats];
            engine->RenderAudio(tempBlock);

            for (size_t i = 0; i < blockFloats; ++i)
            {
                engine->fifoBuffer[engine->fifoWrite] = tempBlock[i];
                engine->fifoWrite = (engine->fifoWrite + 1) % cFifoCapacity;
            }
            engine->fifoCount += blockFloats;
        }

        size_t toCopy = std::min(floatsNeeded, engine->fifoCount);
        for (size_t i = 0; i < toCopy; ++i)
        {
            out[i] = engine->fifoBuffer[engine->fifoRead];
            engine->fifoRead = (engine->fifoRead + 1) % cFifoCapacity;
        }
        engine->fifoCount -= toCopy;

        if (toCopy < floatsNeeded)
            std::fill(out + toCopy, out + floatsNeeded, 0.0f);
    }

    void AudioEngine::SetSourceActive(SourceId id, bool active)
    {
        auto it = sources.find(id);
        if (it != sources.end())
            it->second->SetActive(active);
    }

    void AudioEngine::SetSourceParam(SourceId id, const std::string& name, float value)
    {
        auto it = sources.find(id);
        if (it != sources.end())
            it->second->SetParam(name, value);
    }

    void AudioEngine::SetSourcePosition(SourceId id, IPLVector3 pos)
    {
        auto it = sources.find(id);
        if (it != sources.end())
            it->second->SetPosition(pos);
    }

    SourceId AudioEngine::SpawnSource(std::unique_ptr<dsp> faustDsp, IPLVector3 pos, bool autoStart)
    {
        auto src = std::make_unique<SoundSource>();
        src->Init(mAcousticWorld.GetContext(), mAcousticWorld.GetHRTF(), mAcousticWorld.GetSimulator(), std::move(faustDsp));
        src->SetPosition(pos);
        src->SetActive(autoStart);
        SourceId id = mSourceId++;

        if (mSDL_AudioDeviceID != 0)
            SDL_LockAudioDevice(mSDL_AudioDeviceID);

        sources.emplace(id, std::move(src));

        if (mSDL_AudioDeviceID != 0)
            SDL_UnlockAudioDevice(mSDL_AudioDeviceID);

        iplSimulatorCommit(mAcousticWorld.GetSimulator());

        return id;
    }

    bool AudioEngine::Start()
    {
        SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

        if ((SDL_WasInit(SDL_INIT_AUDIO) == 0) && (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0))
        {
            std::cerr << "SDL_InitSubSystem(AUDIO) failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_AudioSpec want{}, have{};
        want.freq = cAudioSampleRate;
        want.format = AUDIO_F32SYS;
        want.channels = 2;
        want.samples = cAudioFrameSize;
        want.callback = &AudioEngine::SdlCallback;
        want.userdata = this;

        mSDL_AudioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
        if (mSDL_AudioDeviceID == 0)
        {
            std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
            return false;
        }

        std::cout
                << "[SDL2] Opened: freq=" << have.freq
                << " ch=" << (int)have.channels
                << " samples=" << have.samples
                << " format=" << have.format << "\n";

        SDL_PauseAudioDevice(mSDL_AudioDeviceID, 0);

        return true;
    }

    void AudioEngine::Stop()
    {
        if (mSDL_AudioDeviceID != 0)
        {
            SDL_CloseAudioDevice(mSDL_AudioDeviceID);
            mSDL_AudioDeviceID = 0;
        }
    }

    void AudioEngine::Update(IPLVector3 listenerPos, IPLVector3 listenerAhead, IPLVector3 listenerUp)
    {
        IPLCoordinateSpace3 listener{};
        listener.origin = listenerPos;
        listener.ahead = listenerAhead;
        listener.up = listenerUp;
        listener.right = {
            listenerAhead.y * listenerUp.z - listenerAhead.z * listenerUp.y,
            listenerAhead.z * listenerUp.x - listenerAhead.x * listenerUp.z,
            listenerAhead.x * listenerUp.y - listenerAhead.y * listenerUp.x
        };

        for (auto& src : sources)
            src.second->SetSimulationInputs();

        mAcousticWorld.RunDirectSimulation(listener);

        for (auto& src : sources)
            src.second->ReadSimulationOutputs(listener);
    }
}
