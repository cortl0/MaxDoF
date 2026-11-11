// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraphStructures.h"

namespace MaxDoF::Common::Maps
{
    class DeathmatchSpaceGraphGenerator
    {
    public:
        DeathmatchSpaceGraphGenerator() = delete;

        /**
         * @brief Генератор идеального симметричного графа для режима Deathmatch (Бой на смерть).
         *
         * Создает центральную залу-тетраэдр (NodeType::Tetrahedron) радиусом 50 метров.
         * От каждой из 4-х граней залы наружу прокладываются 4 сопряженных туннеля (NodeType::PipeTunnel)
         * длиной 10 метров. Внутренние стыки остаются открытыми проходами, а внешние шлюзы
         * герметично запечатываются глухой породой.
         *
         * Геометрия и winding order флагов настроены пассивно и предсказуемо.
         *
         * @param graph Ссылка на структуру графа уровня для записи.
         */
        static void GenerateGraph(Common::Types::LevelGraphs::PureLevelGraph& graph);
    };
}
