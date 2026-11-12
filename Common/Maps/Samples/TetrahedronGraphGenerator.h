// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraphStructures.h"

namespace MaxDoF::Common::Maps
{
    class TetrahedronGraphGenerator
    {
    public:
        // Запрещаем создавать экземпляры класса, так как это чистый генератор-утилита
        TetrahedronGraphGenerator() = delete;

        /**
         * @brief Генератор идеального симметричного графа для режима Deathmatch (Бой на смерть).
         *
         * @param graph Ссылка на структуру графа уровня, куда будут записаны узлы и интерфейсы.
         */
        static void GenerateGraph(Types::LevelGraphs::PureLevelGraph& graph, JPH::RVec3 offset);
    };
}
