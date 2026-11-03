// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include <iostream>

#define MaxDoF_LOG_AND_ABORT(msg) \
    std::cout << std::endl \
        << "Error in file: " << __FILE__ << std::endl \
        << "line: " << __LINE__ << std::endl \
        << "message: " << msg << std::endl << std::endl; \
    std::abort();
