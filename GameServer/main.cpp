#include <iostream>

#include "version.h"

#include "GameServer/MdfServer.h"

int main()
{
    std::cout << "MaxDoF Server Version: " << MaxDoF_VERSION_STRING << std::endl;
    MaxDoF::GameServer::MdfServer mMdfServer;
#ifdef MAXDOF_CLIENT_BUILD
    std::cout << "Hello, World!" << std::endl;
#endif
    return 0;
}
