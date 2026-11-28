// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

import("stdfaust.lib");

// Элементы управления
gate   = button("engage"); // Включение/выключение тяги
thrust = hslider("thrust", 0.5, 0.0, 1.0, 0.01) : si.smoo; // Мощность тяги
gain   = hslider("gain", 0.7, 0.0, 2.0, 0.01) : si.smoo;

// Плавный разгон и торможение
engineState = gate : si.smooth(ba.tau2pole(1.0));
power       = engineState * (0.15 + thrust * 0.85);

// 1. Глубокий саб-бас (давление турбины в районе 45-85 Гц, без синусоид)
subFreq     = 45.0 + power * 40.0;
subPressure = no.pink_noise : fi.resonlp(subFreq, 2.5, 1.0) : fi.lowpass(2, 95.0) * 2.2;

// 2. Плотный низкочастотный поток (основное "тело" тяги)
lowFlow     = no.pink_noise : fi.lowpass(3, 180.0 + power * 320.0) * 1.6;

// 3. Воздушный свист сопла
whistleFreq = 750.0 + (power * power) * 1900.0;
airWhistle  = no.noise : fi.resonbp(whistleFreq, 5.5, 0.30);

// 4. Шипение плазмы/пара
hissLow     = 1500.0 + power * 1000.0;
hissHigh    = 4500.0 + power * 4500.0;
jetHiss     = no.noise : fi.bandpass(2, hissLow, hissHigh) * (0.25 + power * 0.75);

// Сборка: сначала слегка перегружаем низы для "жира", затем смешиваем со свистом и шипением
lowEnd      = (subPressure + lowFlow) : ma.tanh;
engineMix   = (lowEnd * 1.15 + airWhistle + jetHiss) * engineState;

process     = engineMix : ma.tanh * gain;
