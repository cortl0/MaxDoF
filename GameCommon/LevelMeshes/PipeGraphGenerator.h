// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "LevelGraphStructures.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    class PipeGraphGenerator
    {
    public:
        PipeGraphGenerator() = delete;

        /**
         * @brief Генератор изолированной тупиковой трубы с глухими торцами.
         *
         * Строит одиночный коридор-антипризму длиной 10 метров вдоль оси Z.
         * Выходной торец закручен на 30 градусов, образуя в центре идеальный шестигранник.
         * Торцы запечатаны глухими стенами.
         *
         * @param graph Ссылка на структуру графа уровня для записи.
         */
        static void GenerateGraph(MaxDoF::GameCommon::LevelGraphs::PureLevelGraph& graph);
    };
}

