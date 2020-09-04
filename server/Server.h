#ifndef __SERVER
#define __SERVER

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <queue>
#include <map>

using namespace std;

class Server{

private:
    const static int WORKER_THREAD_COUNT = 45;
    const static int CLIENT_SESSION_THREAD_COUNT = 45;
    const static int CLIENT_INITIALIZER_THREAD_COUNT = 10;

    struct WorkerThread{
        queue<int> requestSocketIDs;
        thread t;
    };
    struct ClientSessionThread{
        queue<int> clients;
        thread t;
    };
    struct ClientInitializerThread{
        queue<int> clientSocketIDs;
        thread t;
    };
    struct SessionCleanerThread{
        queue<int> sessionIDs;
        map<int, bool> isSessionActive;
        thread t;
    };

    WorkerThread workerThreads[WORKER_THREAD_COUNT];
    ClientSessionThread clientSesssionThreads[CLIENT_SESSION_THREAD_COUNT];
    ClientInitializerThread clientInitializerThread[CLIENT_INITIALIZER_THREAD_COUNT];
    SessionCleanerThread sessionCleanerThread;

    struct sockaddr_in address;
    char *indexHTML;
    int server_fd;

    void initializeThreads();
    void parseRequest(int);
    void processSessionRequest(int);
    void processInitializerRequest(int);
    void cleanSessions();

public:
    void init(int);
    void run();
    void clear(); // TODO clear?

};

#endif // __SERVER