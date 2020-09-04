#include <iostream>
#include "server/Server.h"

using namespace std;

const int PORT = 8088;

int main(int argc, char const **argv) {
    
    // TODO load balancing, so parse main inputs
    
    Server server;

    server.init(PORT);
    server.run();

    return 0;
}

// g++ -o exec *.cpp */*.cpp -pthread