// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include "Tetrahedron.h"

namespace MaxDoF::GameCommon::LevelGraphs
{
    Tetrahedron::Tetrahedron()
    // const JPH::RVec3& pos, const JPH::Quat& ori
    {
        SetVetricesAndInterfacesBySphereRadius(0.5);
    }

    void Tetrahedron::SetVetricesAndInterfacesBySphereRadius(double tetrahedronSphereRadius)
    {
        constexpr double sqrt3 = 1.7320508;
        constexpr double sqrt6 = 2.4494897;

        // Длина ребра правильного тетраэдра при заданном радиусе описанной сферы
        const double edgeLength = (4.0 / sqrt6) * tetrahedronSphereRadius;

        // Радиус описанной окружности грани (размер прохода трубы)
        const double gateSize = edgeLength / sqrt3;

        // Расчет пространственного положения плоскостей для Z-ориентированного тетраэдра
        const double zBack = tetrahedronSphereRadius;            // Одиночная вершина сзади (в плюсе по Z)
        const double zFront = -tetrahedronSphereRadius / 3.0;    // Плоскость основания впереди (в минусе по Z)

        // Высоты для правильного треугольника основания на фронтальной плоскости XY
        const double yTop = gateSize;                  // Верхняя точка основания (смотрит вверх по Y)
        const double yBottom = -gateSize / 2.0;       // Нижняя линия основания

        vertices = {{
            JPH::RVec3(0.0f, 0.0f, zBack),                     // Вершина 0 (Сзади, на оси +Z)
            JPH::RVec3(0.0f, yTop, zFront),                    // Вершина 1 (Вперед-вверх)
            JPH::RVec3(-edgeLength / 2.0f, yBottom, zFront),   // Вершина 2 (Вперед-влево-вниз)
            JPH::RVec3(edgeLength / 2.0f, yBottom, zFront)     // Вершина 3 (Вперед-вправо-вниз)
        }};

        interfaces = {{
            {0, 2, 1}, // Грань 0 Левая
            {0, 1, 3}, // Грань 1 Правая
            {0, 3, 2}, // Грань 2 Нижняя
            {1, 2, 3}  // Грань 3 Переднияя
        }};
    }
}
