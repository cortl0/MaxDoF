#include <iostream>

#include "server.h"

int main()
{
    MaxDoF::Server::Server server;
#ifdef MAXDOF_CLIENT_BUILD
    std::cout << "Hello, World!" << std::endl;
#endif
    return 0;
}
