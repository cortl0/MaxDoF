// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <random>

#include "Config.h"

namespace MaxDoF::Game::Textures
{
    class ConfigFactory
    {
        static std::mt19937 make_seeded_rng();
    public:
        // Генерирует случайную конфигурацию.
        // sizePower: показатель степени двойки [1 .. 30] (по умолчанию 8 -> 2^8 = 256).
        // detailLevel: 0 = крупные формы, 1+ = мельче.
        static Config CreateRandom(
            uint32_t seed,
            uint32_t sizePower = 8,
            uint32_t detailLevel = 0
        );
    };
}
