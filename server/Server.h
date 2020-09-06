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

/**
 * Core part of the server
 * Runs with 5 main type thread:
 *  - main thread: runs on run()
 *  - worker threads: run on parseRequest()
 *  - client initializer threads: run on processInitializerRequest()
 *  - client session threads: run on processSessionRequest()
 *  - session management thread: runs on cleanSessions()
 * detailed information can be found on the related methods
 */
class Server{

private:
    const static int WORKER_THREAD_COUNT = 45;
    const static int CLIENT_INITIALIZER_THREAD_COUNT = 20;
    const static int CLIENT_SESSION_THREAD_COUNT = 35;

    struct WorkerThread{
        queue<int> requestSocketIDs;
        thread t;
    };
    struct ClientInitializerThread{
        queue<Request> requests; 
        thread t;
    };
    struct ClientSessionThread{
        queue<int> clients;
        thread t;
    };
    struct SessionManagementThread{
         // first param indicates if the session is active
         // second param is the pointer to the session
        map<int, bs> sessions;
        queue<int> activeSessionIDs;
        thread t;
    };

    WorkerThread workerThreads[WORKER_THREAD_COUNT];
    ClientInitializerThread clientInitializerThread[CLIENT_INITIALIZER_THREAD_COUNT];
    ClientSessionThread clientSesssionThreads[CLIENT_SESSION_THREAD_COUNT];
    SessionManagementThread sessionManagementThread;

    struct sockaddr_in address;
    int server_fd;

    void initializeThreads();
    void parseRequest(int);
    void processInitializerRequest(int);
    void processSessionRequest(int);
    void cleanSessions();
    int chooseSessionThread();

public:
    void init(int);
    void run();

};

#endif // __SERVER