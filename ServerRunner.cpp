#include <iostream>
#include "Server.h"

using namespace std;

const int PORT = 8088;

int main(int argc, char const **argv) {
    
    Server server;

    server.init(PORT);
    server.run();

    return 0;
}