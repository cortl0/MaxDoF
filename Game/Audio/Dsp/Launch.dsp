// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

import("stdfaust.lib");

gate  = button("gate");
gain  = hslider("gain", 0.9, 0.0, 2.0, 0.01) : si.smoo;
pitch = hslider("pitch", 2200.0, 400.0, 5000.0, 10.0) : si.smoo;

// 1. Огибающие
pitchEnv = en.ar(0.001, 0.015, gate);
bodyEnv  = en.ar(0.001, 0.035, gate);

// 2. Механический удар затвора / реле
tickBody = os.osc(pitch * (0.45 + 0.55 * pitchEnv)) * bodyEnv * 0.85;

// 3. Четкий щелчок пружины/искры (используем fi.resonbp: частота, Q=3.0, громкость=1.0)
noiseEnv  = en.ar(0.001, 0.020, gate);
tickNoise = no.noise : fi.resonbp(pitch * 1.2, 3.0, 1.0) * noiseEnv * 0.75;

// Насыщение и защита от выхода за пределы [-1, 1]
process = (tickBody + tickNoise) : ma.tanh * gain;
