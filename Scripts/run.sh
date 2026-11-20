#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "${SCRIPT_DIR}" || exit $?
MAXDOF_PATH=$(pwd)/..

# Uncomment what you want

# cd ${MAXDOF_PATH}/bin/linux-x86_64/Debug
# cd ${MAXDOF_PATH}/bin/linux-x86_64/Release
# ./MaxDoFServer
# ./MaxDoFGame

# cd ${MAXDOF_PATH}/bin/windows-x86_64/Debug
# cd ${MAXDOF_PATH}/bin/windows-x86_64/Release
# wine ./MaxDoFServer.exe
# wine ./MaxDoFGame.exe
