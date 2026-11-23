// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "ConfigFactory.h"

#include <array>
#include <algorithm>

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Game::Textures
{
    std::mt19937 ConfigFactory::make_seeded_rng()
    {
        std::random_device rd;
        std::array<std::uint32_t, std::mt19937::state_size> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());
        return std::mt19937(seq);
    }

    Config ConfigFactory::CreateRandom(uint32_t seed, uint32_t sizePower, uint32_t detailLevel)
    {
        // Нарушение теоретического минимума (2^1 = 2) или переполнение uint32_t
        if (sizePower < 1u || sizePower >= 31u)
            MaxDoF_LOG_AND_ABORT("bad data")

        std::mt19937 rng(seed);
        // std::mt19937 rng(make_seeded_rng());//(seed);

        // 4 формулы: [0 .. 2^2 - 1]
        std::uniform_int_distribution<uint32_t> formulaDist(0u, (1u << 2) - 1u);

        // Смещение от 0 до 2^(sizePower - 1)
        const uint32_t maxOffset = 1u << (sizePower - 1u);
        std::uniform_int_distribution<uint32_t> offsetDist(0u, maxOffset);

        const uint32_t maxShift = sizePower - 1u;
        const uint32_t minShift = (maxShift > (1u + detailLevel)) ? (maxShift - 1u - detailLevel) : 0u;
        std::uniform_int_distribution<uint32_t> shiftDist(minShift, maxShift);

        std::uniform_int_distribution<int32_t> hueDist(0, 359);
        std::uniform_int_distribution<int32_t> stepDist(-45, 45);

        Config cfg;
        cfg.size_power  = sizePower;
        cfg.formula     = static_cast<FormulaType>(formulaDist(rng));
        cfg.offset_a    = offsetDist(rng);
        cfg.offset_b    = offsetDist(rng);
        cfg.quant_shift = shiftDist(rng);
        // cfg.base_hue    = hueDist(rng);
        // cfg.hue_step    = stepDist(rng);

        cfg.Print();

        return cfg;
    }
}
