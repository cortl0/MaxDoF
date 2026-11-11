// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#include <iostream>

#include "version.h"

#include "Server/MdfServer.h"

int main()
{
    std::cout << "MaxDoF Server Version: " << MaxDoF_VERSION_STRING << std::endl;
    MaxDoF::Server::MdfServer mMdfServer;
#ifdef MAXDOF_CLIENT_BUILD
    std::cout << "Hello, World!" << std::endl;
#endif
    return 0;
}
