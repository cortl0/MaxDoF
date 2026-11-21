// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <cstdint>

namespace MaxDoF::Game::Textures
{
    struct RGBA
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;
    };

    enum class FormulaType : uint8_t
    {
        XOR_CRYSTAL    = 0, // Фрактальные треугольники и сетки
        TECH_MASK      = 1, // Угловатые схемы и лабиринты
        CONCENTRIC     = 2, // Круги и муаровые решетки
        DIAGONAL_SLICE = 3  // Рубленые слои и шевроны
    };

    // Детерминированные параметры генерации.
    // Разрешение текстуры строго равно (1u << size_power).
    // Теоретический минимум: size_power >= 1 (2^1 = 2 пикселя).
    struct Config
    {
        uint32_t size_power  = 8;   // Показатель степени двойки: 8 -> 2^8 = 256. Диапазон: [1 .. 30]
        FormulaType formula  = FormulaType::XOR_CRYSTAL;
        uint32_t offset_a    = 0;   // Фазовое смещение A: [0 .. 2^(size_power - 1)]
        uint32_t offset_b    = 0;   // Фазовое смещение B: [0 .. 2^(size_power - 1)]
        uint32_t quant_shift = 6;   // Квантование (отсечение мелких деталей): [0 .. size_power - 1]
        int32_t base_hue     = 0;   // Базовый тон HSV: [0 .. 359]
        int32_t hue_step     = 25;  // Шаг сдвига тона: [-180 .. 180]

        constexpr uint32_t GetSize() const noexcept { return 1u << size_power; }
    };
}
