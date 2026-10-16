#include <iostream>

#include "GameServer/MdfServer.h"

int main()
{
    MaxDoF::GameServer::MdfServer mMdfServer;
#ifdef MAXDOF_CLIENT_BUILD
    std::cout << "Hello, World!" << std::endl;
#endif
    return 0;
}
