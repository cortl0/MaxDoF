// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

import("stdfaust.lib");

freq = hslider("freq", 220.0, 50.0, 2000.0, 1.0) : si.smoo;
gain = hslider("gain", 0.4, 0.0, 1.0, 0.01) : si.smoo;

process = os.sawtooth(freq) : fi.lowpass(2, 6000) * gain;
