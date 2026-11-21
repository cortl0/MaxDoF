// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <array>
#include <cstdint>

#include <OgreImage2.h>
#include <OgreString.h>

#include "Config.h"

namespace MaxDoF::Game::Textures
{
    class Generator
    {
    public:
        Ogre::Image2 GenerateImage(const Config& cfg) const;
        void SaveToPng(const Config& cfg, const Ogre::String& filepath) const;

    private:
        uint32_t ComputeFormula(uint32_t x, uint32_t y, const Config& cfg) const;
        std::array<RGBA, 4> BuildPalette(int32_t baseHue, int32_t hueStep) const;
        RGBA HsvToRgba(int32_t h, int32_t s, int32_t v) const;
    };
}
