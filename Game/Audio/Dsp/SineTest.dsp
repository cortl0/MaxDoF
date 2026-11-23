// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

import("stdfaust.lib");

gate     = button("gate");
gain     = hslider("gain", 0.8, 0.0, 2.0, 0.01) : si.smoo;
// Можно менять размер/высоту бочки от 0.5 (маленькая канистра) до 1.5 (большая цистерна)
size     = hslider("size", 1.0, 0.4, 2.0, 0.01) : si.smoo;
// Сила удара (влияет на яркость звона и время затухания)
velocity = hslider("velocity", 0.8, 0.1, 1.0, 0.01) : si.smoo;

// Базовая частота (зависит от размера объекта)
f0 = 135.0 / size;

// 1. Импульс удара (короткий жесткий стук в момент столкновения)
impEnv  = en.ar(0.0005, 0.012, gate);
impulse = (no.noise : fi.bandpass(2, 400.0, 3500.0 * velocity + 500.0)) * impEnv * 1.8;

// 2. Пустой внутренний объем бочки (низкий гулкий "бум" с быстрым затуханием)
bodyEnv = en.ar(0.002, 0.18 * size, gate);
body    = os.osc(f0 * (1.0 + 0.15 * bodyEnv)) * bodyEnv * 0.9;

// 3. Негармонические моды металла (характерный "жестяной" звон бочки)
// Соотношения частот для цилиндрической оболочки: 1.0, 1.58, 2.14, 2.76, 3.41, 4.18
decay = (0.35 + velocity * 0.45) * size;

mode(ratio, amp, dScale) = impulse : fi.resonbp(f0 * ratio, 45.0 + velocity * 35.0, amp) * en.ar(0.001, decay * dScale, gate);

metalRing = mode(1.58, 0.7, 1.0)
          + mode(2.14, 0.6, 0.85)
          + mode(2.76, 0.5, 0.70)
          + mode(3.41, 0.4, 0.55)
          + mode(4.18, 0.3, 0.40)
          + mode(5.32, 0.2, 0.25);

// 4. Сборка и легкая сатурация металла
mix     = (impulse * 0.8 + body + metalRing * (0.6 + velocity * 0.6)) * velocity;
process = mix : ma.tanh * gain;
