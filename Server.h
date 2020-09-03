#ifndef __SERVER
#define __SERVER

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

class Server{

private:
    int server_fd;
    struct sockaddr_in address;
    char *indexHTML;

public:
    void init(int);
    void run();
    void clear();

};

#endif // __SERVER