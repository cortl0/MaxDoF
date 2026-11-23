// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "SoundSource.h"

#include <cmath>

namespace MaxDoF::Game::Audio
{
    SoundSource::~SoundSource()
    {
        if (mIPLBinauralEffect)
            iplBinauralEffectRelease(&mIPLBinauralEffect);

        if (mIPLDirectEffect)
            iplDirectEffectRelease(&mIPLDirectEffect);

        if (mIPLSource && mIPLSimulator)
        {
            iplSourceRemove(mIPLSource, mIPLSimulator);
            iplSourceRelease(&mIPLSource);
        }
    }

    void SoundSource::Init(IPLContext ctx, IPLHRTF hrtf, IPLSimulator simulator, std::unique_ptr<dsp> faustDsp)
    {
        mIPLContext = ctx;
        mIPLHRTF = hrtf;
        mIPLSimulator = simulator;
        mDsp = std::move(faustDsp);

        mDsp->init(cAudioSampleRate);
        mDsp->buildUserInterface(&mMapUI);

        // Сохраняем дефолтные значения всех параметров
        numCachedParams = std::min(mMapUI.getParamsCount(), cMaxSourceParams);
        for (int i = 0; i < numCachedParams; ++i)
            m_cachedParams.at(i) = { i, mMapUI.getParamValue(mMapUI.getParamAddress(i)) };

        faustMem.resize(cAudioFrameSize, 0.0f);
        directMem.resize(cAudioFrameSize, 0.0f);
        stereoMem.resize(cAudioFrameSize * 2, 0.0f);

        inPtr = faustMem.data();
        inBuf = { 1, cAudioFrameSize, &inPtr };

        directPtr = directMem.data();
        directBuf = { 1, cAudioFrameSize, &directPtr };

        stereoPtrs[0] = stereoMem.data();
        stereoPtrs[1] = stereoMem.data() + cAudioFrameSize;
        stereoBuf = { 2, cAudioFrameSize, stereoPtrs };

        IPLAudioSettings audioSettings{ cAudioSampleRate, cAudioFrameSize };

        IPLDirectEffectSettings directSettings{ 1 };
        iplDirectEffectCreate(mIPLContext, &audioSettings, &directSettings, &mIPLDirectEffect);

        IPLBinauralEffectSettings binauralSettings{ mIPLHRTF };
        iplBinauralEffectCreate(mIPLContext, &audioSettings, &binauralSettings, &mIPLBinauralEffect);

        IPLSourceSettings srcSettings{ IPL_SIMULATIONFLAGS_DIRECT };
        iplSourceCreate(mIPLSimulator, &srcSettings, &mIPLSource);
        iplSourceAdd(mIPLSource, mIPLSimulator);
    }

    void SoundSource::ReadSimulationOutputs(const IPLCoordinateSpace3& listener)
    {
        SourceRenderState state{};
        state.active = isActive;
        state.params = m_cachedParams;
        state.numParams = numCachedParams;

        if (isActive)
        {
            IPLSimulationOutputs outputs{};
            iplSourceGetOutputs(mIPLSource, IPL_SIMULATIONFLAGS_DIRECT, &outputs);
            state.directParams = outputs.direct;

            state.directParams.transmissionType = IPL_TRANSMISSIONTYPE_FREQDEPENDENT;
            state.directParams.flags = static_cast<IPLDirectEffectFlags>(
                IPL_DIRECTEFFECTFLAGS_APPLYDISTANCEATTENUATION |
                IPL_DIRECTEFFECTFLAGS_APPLYOCCLUSION
            );

            // Считаем расстояние от слушателя до источника
            float dx = position.x - listener.origin.x;
            float dy = position.y - listener.origin.y;
            float dz = position.z - listener.origin.z;
            float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

            // Плавно сводим 3D-эффект в ноль при сближении ближе 1.5 метров
            constexpr float blendRadius = 1.5f;
            state.spatialBlend = std::clamp(dist / blendRadius, 0.0f, 1.0f);

            // Если дистанция почти нулевая, оставляем безопасный вектор направления, чтобы не делить на 0
            if (dist > 1e-4f)
            {
                state.direction = iplCalculateRelativeDirection(
                    mIPLContext, position, listener.origin, listener.ahead, listener.up
                );
            } else {
                state.direction = {0.0f, 0.0f, -1.0f};
            }
        }

        stateBuffer.Write(state);
    }

    void SoundSource::RenderAndMix(IPLAudioBuffer& stereoMixOut)
    {
        const SourceRenderState& state = stateBuffer.Read();
        if (!state.active || !mDsp)
        {
            hasSmoothedInit = false;
            return;
        }

        if (!hasSmoothedInit)
        {
            smoothedDir = state.direction;
            smoothedAtten = state.directParams.distanceAttenuation;
            smoothedOcc = state.directParams.occlusion;
            smoothedBlend = state.spatialBlend;
            hasSmoothedInit = true;
        }

        constexpr float alpha = 0.25f;
        smoothedDir.x += (state.direction.x - smoothedDir.x) * alpha;
        smoothedDir.y += (state.direction.y - smoothedDir.y) * alpha;
        smoothedDir.z += (state.direction.z - smoothedDir.z) * alpha;

        float len = std::sqrt(smoothedDir.x * smoothedDir.x +
                            smoothedDir.y * smoothedDir.y +
                            smoothedDir.z * smoothedDir.z);
        if (len > 1e-5f)
        {
            smoothedDir.x /= len;
            smoothedDir.y /= len;
            smoothedDir.z /= len;
        }
        else
        {
            smoothedDir = {0.0f, 0.0f, -1.0f};
        }

        smoothedAtten += (state.directParams.distanceAttenuation - smoothedAtten) * alpha;
        smoothedOcc   += (state.directParams.occlusion - smoothedOcc) * alpha;
        smoothedBlend += (state.spatialBlend - smoothedBlend) * alpha;

        IPLDirectEffectParams smoothDirect = state.directParams;
        smoothDirect.distanceAttenuation = smoothedAtten;
        smoothDirect.occlusion = smoothedOcc;

        // Применяем параметры Faust
        for (int i = 0; i < state.numParams; ++i)
        {
            const auto& p = state.params.at(i);
            if (p.paramIndex >= 0)
                mMapUI.setParamValue(mMapUI.getParamAddress(p.paramIndex), p.value);
        }

        // Синтез Faust
        mDsp->compute(cAudioFrameSize, nullptr, &inPtr);

#if(0)
        for (int i = 0; i < cAudioFrameSize; ++i)
            if (!std::isfinite(inPtr[i]))
                inPtr[i] = 0.0f;
#endif

        // Direct эффект
        iplDirectEffectApply(mIPLDirectEffect, &smoothDirect, &inBuf, &directBuf);

        IPLBinauralEffectParams binParams{};
        binParams.direction = smoothedDir;
        binParams.interpolation = IPL_HRTFINTERPOLATION_BILINEAR;
        binParams.spatialBlend = smoothedBlend;
        binParams.hrtf = mIPLHRTF;

        iplBinauralEffectApply(mIPLBinauralEffect, &binParams, &directBuf, &stereoBuf);

        // Смешиваем левый [0] и правый [1] каналы
        for (int i = 0; i < cAudioFrameSize; ++i)
        {
            stereoMixOut.data[0][i] += stereoPtrs[0][i];
            stereoMixOut.data[1][i] += stereoPtrs[1][i];
        }
    }

    void SoundSource::SetActive(bool active)
    {
        if (active && !isActive && mDsp)
            mDsp->instanceClear();

        isActive = active;
    }

    void SoundSource::SetParam(const std::string& path, float value)
    {
        for (int i = 0; i < numCachedParams; ++i)
        {
            std::string addr = mMapUI.getParamAddress(i);
            if (addr == path || (addr.size() > path.size() &&
                addr.compare(addr.size() - path.size(), path.size(), path) == 0 &&
                addr.at(addr.size() - path.size() - 1) == '/'))
            {
                m_cachedParams.at(i).value = value;
            }
        }
    }

    void SoundSource::SetSimulationInputs()
    {
        if (!isActive)
            return;

        IPLSimulationInputs inputs{};
        inputs.flags = IPL_SIMULATIONFLAGS_DIRECT;
        inputs.directFlags = static_cast<IPLDirectSimulationFlags>(
            IPL_DIRECTSIMULATIONFLAGS_DISTANCEATTENUATION |
            IPL_DIRECTSIMULATIONFLAGS_OCCLUSION |
            IPL_DIRECTSIMULATIONFLAGS_TRANSMISSION
        );
        inputs.source.origin = position;
        inputs.source.ahead = {0.0f, 0.0f, -1.0f};
        inputs.source.up = {0.0f, 1.0f, 0.0f};
        inputs.source.right = {1.0f, 0.0f, 0.0f};

        inputs.distanceAttenuationModel.type = IPL_DISTANCEATTENUATIONTYPE_INVERSEDISTANCE;
        inputs.distanceAttenuationModel.minDistance = 2.0f;

        inputs.occlusionType = IPL_OCCLUSIONTYPE_VOLUMETRIC;
        inputs.occlusionRadius = 0.05f;
        inputs.numOcclusionSamples = 16;

        iplSourceSetInputs(mIPLSource, IPL_SIMULATIONFLAGS_DIRECT, &inputs);
    }
}
