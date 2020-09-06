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
#include <mutex>
#include <ctime>

#include "Session.h"

typedef pair<bool, Session *> bs;

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
        queue<Request> requests; 
        thread t;
    };
    struct SessionManagementThread{
        queue<int> activeSessionIDs;
         // first param indicates if the session is active, second param is the pointer for the session
        map<int, bs> sessions;
        thread t;
    };

    WorkerThread workerThreads[WORKER_THREAD_COUNT];
    ClientSessionThread clientSesssionThreads[CLIENT_SESSION_THREAD_COUNT];
    ClientInitializerThread clientInitializerThread[CLIENT_INITIALIZER_THREAD_COUNT];
    SessionManagementThread sessionManagementThread;

    struct sockaddr_in address;
    int server_fd;

    void initializeThreads();
    int chooseSessionThread();
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