#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

SCRIPT_DIR=$(dirname "$0")
cd "${SCRIPT_DIR}" || exit $?

for PLATFORM in "linux-x86_64" "windows-x86_64"; do
    for CONFIG in "Debug" "Release"; do
        ./prepare_dependencies_and_build.sh "${PLATFORM}" "${CONFIG}" || exit $?
    done
done
