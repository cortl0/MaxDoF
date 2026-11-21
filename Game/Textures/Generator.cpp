// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Generator.h"

#include <OgreMemoryAllocatorConfig.h>
#include <OgrePixelFormatGpu.h>
#include <OgrePixelFormatGpuUtils.h>

#if WIN32
#include <OgreTextureBox.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#endif

#include "Common/Utils/SystemUtils.h"

namespace MaxDoF::Game::Textures
{
    Ogre::Image2 Generator::GenerateImage(const Config& cfg) const
    {
        // Теоретический минимум: 2^1 = 2 пикселя; максимум для uint32_t: 2^30
        if (cfg.size_power < 1u || cfg.size_power >= 31u)
            MaxDoF_LOG_AND_ABORT("bad data")

        const uint32_t size      = cfg.GetSize();               // 2^size_power
        const uint32_t halfSize  = 1u << (cfg.size_power - 1u); // 2^(size_power - 1)
        const uint32_t maxShift  = cfg.size_power - 1u;

        if (cfg.offset_a > halfSize || cfg.offset_b > halfSize || cfg.quant_shift > maxShift)
            MaxDoF_LOG_AND_ABORT("bad data")

        const std::array<RGBA, 4> palette = BuildPalette(cfg.base_hue, cfg.hue_step);

        const size_t rowAlignment = 4;
        const size_t dataSize = Ogre::PixelFormatGpuUtils::calculateSizeBytes(
            size, size, 1, 1, Ogre::PFG_RGBA8_UNORM_SRGB, 1, rowAlignment
        );

        uint8_t* rawData = reinterpret_cast<uint8_t*>(
            OGRE_MALLOC_SIMD(dataSize, Ogre::MEMCATEGORY_RESOURCE)
        );
        uint32_t* pixels = reinterpret_cast<uint32_t*>(rawData);

        constexpr uint32_t kPaletteMask = (1u << 2) - 1u; // 2^2 = 4 цвета (маска 0b11)

        for (uint32_t y = 0; y < size; ++y)
        {
            const uint32_t sy = (y < halfSize) ? y : (size - 1u - y);

            for (uint32_t x = 0; x < size; ++x)
            {
                const uint32_t sx = (x < halfSize) ? x : (size - 1u - x);

                const uint32_t rawVal    = ComputeFormula(sx, sy, cfg);
                const uint32_t quantized = rawVal >> cfg.quant_shift;
                const RGBA& c            = palette[quantized & kPaletteMask];

                pixels[y * size + x] =
                    (static_cast<uint32_t>(c.a) << 24) |
                    (static_cast<uint32_t>(c.b) << 16) |
                    (static_cast<uint32_t>(c.g) << 8)  |
                    static_cast<uint32_t>(c.r);
            }
        }

        Ogre::Image2 image;
        image.loadDynamicImage(
            rawData,
            size,
            size,
            1,
            Ogre::TextureTypes::Type2D,
            Ogre::PFG_RGBA8_UNORM_SRGB,
            true,
            1
        );

        return image;
    }

    void Generator::SaveToPng(const Config& cfg, const Ogre::String& filepath) const
    {
        Ogre::Image2 img = GenerateImage(cfg);
#if WIN32
        // Получаем данные нулевого (базового) мипмапа
        Ogre::TextureBox box = img.getData(0);

        const int width  = static_cast<int>(box.width);
        const int height = static_cast<int>(box.height);

        // Определяем количество каналов по формату пикселей (обычно 3 для RGB или 4 для RGBA)
        const int channels = static_cast<int>(
            Ogre::PixelFormatGpuUtils::getNumberOfComponents(img.getPixelFormat())
        );

        // box.bytesPerRow учитывает возможное выравнивание строк в памяти (stride)
        const int stride_in_bytes = static_cast<int>(box.bytesPerRow);

        // Сохраняем в PNG (последний аргумент — шаг строки в байтах)
        int result = stbi_write_png(
            filepath.c_str(),
            width,
            height,
            channels,
            box.data,
            stride_in_bytes
        );

        if (result == 0)
        {
            // Ошибка записи файла
        }
#else
        img.save(filepath, 0, 1);
#endif
    }

    uint32_t Generator::ComputeFormula(uint32_t x, uint32_t y, const Config& cfg) const
    {
        const uint32_t a = cfg.offset_a;
        const uint32_t b = cfg.offset_b;

        switch (cfg.formula) {
            case FormulaType::XOR_CRYSTAL:
                return (x + a) ^ (y + b);

            case FormulaType::TECH_MASK:
                return ((x + a) ^ (y + b)) + ((x & y) ^ a);

            case FormulaType::CONCENTRIC:
            {
                const uint32_t normShift = (cfg.size_power > 1u) ? (cfg.size_power - 1u) : 0u;
                const uint64_t dx = static_cast<uint64_t>(x + a);
                const uint64_t dy = static_cast<uint64_t>(y + b);
                return static_cast<uint32_t>((dx * dx + dy * dy) >> normShift);
            }

            case FormulaType::DIAGONAL_SLICE:
                return ((x + a) + (y + b)) ^ ((x + b) ^ (y + a));
        }
        std::abort();
    }

    std::array<RGBA, 4> Generator::BuildPalette(int32_t baseHue, int32_t hueStep) const
    {
        const RGBA bg     = HsvToRgba(baseHue,               75, 18);
        const RGBA mid    = HsvToRgba(baseHue + hueStep,     65, 55);
        const RGBA accent = HsvToRgba(baseHue + hueStep * 2, 45, 95);

        return { bg, bg, mid, accent };
    }

    RGBA Generator::HsvToRgba(int32_t h, int32_t s, int32_t v) const
    {
        h = (h % 360 + 360) % 360;
        s = std::clamp(s, 0, 100);
        v = std::clamp(v, 0, 100);

        const uint8_t region = static_cast<uint8_t>(h / 60);
        const uint8_t rem    = static_cast<uint8_t>((h % 60) * 255 / 60);

        const uint8_t val = static_cast<uint8_t>(v * 255 / 100);
        const uint8_t sat = static_cast<uint8_t>(s * 255 / 100);

        const uint8_t p = static_cast<uint8_t>((val * (255 - sat)) >> 8);
        const uint8_t q = static_cast<uint8_t>((val * (255 - ((sat * rem) >> 8))) >> 8);
        const uint8_t t = static_cast<uint8_t>((val * (255 - ((sat * (255 - rem)) >> 8))) >> 8);

        switch (region)
        {
            case 0:  return { val, t,   p,   255 };
            case 1:  return { q,   val, p,   255 };
            case 2:  return { p,   val, t,   255 };
            case 3:  return { p,   q,   val, 255 };
            case 4:  return { t,   p,   val, 255 };
            default: return { val, p,   q,   255 };
        }
    }
}
