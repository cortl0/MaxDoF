// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

import("stdfaust.lib");

gate = button("gate");
gain = hslider("gain", 0.8, 0.0, 2.0, 0.01) : si.smoo;

// 1. Низкочастотный удар (пинок от 110 Гц вниз до 25 Гц)
pitchEnv = en.ar(0.005, 0.35, gate);
subEnv   = en.ar(0.005, 0.60, gate);
subBody  = os.osc(25.0 + pitchEnv * 85.0) * subEnv * 0.9;

// 2. Грохот и осколки (шум через резонансный фильтр с затуханием 2.2 сек)
rumbleEnv  = en.ar(0.01, 2.20, gate);
cutoff     = 80.0 + (rumbleEnv * rumbleEnv) * 1400.0;
noiseBurst = no.noise : fi.resonlp(cutoff, 1.5, 1.0) * rumbleEnv;

// Мягкое насыщение (сатурация) для плотности взрыва
process = (subBody + noiseBurst) : ma.tanh * gain;
