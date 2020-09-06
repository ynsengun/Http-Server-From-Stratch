#include "Server.h"
#include "ServerUtility.h"
#include "../routes/Routes.h"

const static int REQUEST_BUFFER_SIZE = 1000;
int nextInitializer = 0;

mutex nextInitializerMutex;
mutex sessionRequestPushMutex;
mutex newSessionIDMutex;
mutex activeSessionPushMutex;
mutex sessionPushMutex;

/**
 * after complete init, the socket will be ready to accept connections
 * static pages will be shaped and all threads are initiaized
 */
void Server::init(int port){
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    const int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 100000) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    Routes::init();
    initializeThreads();
}

/**
 * all incoming request are first faced here
 * and the socketIDs are sent to worker threads to parse the request
 */
void Server::run(){
    int addrlen = sizeof(address);
    int new_socket;
    int nextWorker = 0;

    cout<<"Server is running..."<<endl;

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // assign the socket to the next worker thread
        workerThreads[nextWorker].requestSocketIDs.push(new_socket);
        nextWorker++;
        if(nextWorker >= WORKER_THREAD_COUNT){
            nextWorker -= WORKER_THREAD_COUNT;
        }
    }
}

/**
 * all threads are initialized here
 */
void Server::initializeThreads(){
    // worker threads
    for( int i = 0 ; i < WORKER_THREAD_COUNT ; i++ ){
        workerThreads[i].t = thread(&Server::parseRequest, this, i);
    }

    // client session threads
    for( int i = 0 ; i < CLIENT_SESSION_THREAD_COUNT ; i++ ){
        clientSesssionThreads[i].t = thread(&Server::processSessionRequest, this, i);
    }

    // client initializer threads
    for( int i = 0 ; i < CLIENT_INITIALIZER_THREAD_COUNT ; i++ ){
        clientInitializerThread[i].t = thread(&Server::processInitializerRequest, this, i);
    }

    // session cleaner thread
    sessionManagementThread.t = thread(&Server::cleanSessions, this);
}

/**
 * Worker threads run on this method
 * it reads all the incoming request from their sockets and parses the request
 * if the request does contain a sessionID, the parsed request is forwarded to client session thread
 * if does not contain a sessionID, the parsed request is forwarded to client initializer thread
 */
void Server::parseRequest(int tid){
    while(1){
        if(!workerThreads[tid].requestSocketIDs.empty()){
            // get the next request socket id, and read the info in it
            int requestSocketID = workerThreads[tid].requestSocketIDs.front();
            workerThreads[tid].requestSocketIDs.pop();
            char buffer[REQUEST_BUFFER_SIZE];
            read( requestSocketID , buffer, REQUEST_BUFFER_SIZE);

            // parse the request as method, path, sessionID and params(from query string)
            string method = "";
            string path = "";
            int sessionID = -1;
            map<string, string> params;
            ServerUtil::parseRequest(buffer, method, path, sessionID, params);

             // if session is expired
            if(sessionID != -1 && sessionManagementThread.sessions[sessionID].first == false){
                sessionID = -1;
            }

             // forward the request to the next client initilizer thread if does not have sessionID
            if(sessionID == -1 && path != "/favicon.ico"){
                nextInitializerMutex.lock();
                clientInitializerThread[nextInitializer].requests.push(Request{method, path, requestSocketID, params});
                nextInitializer++;
                if(nextInitializer >= CLIENT_INITIALIZER_THREAD_COUNT){
                    nextInitializer -= CLIENT_INITIALIZER_THREAD_COUNT;
                }
                nextInitializerMutex.unlock();
            } else{ // adds the request to the related session
                bs session = sessionManagementThread.sessions[sessionID];

                if(session.first){
                    sessionRequestPushMutex.lock();
                    session.second->pushNewRequest((Request){method, path, requestSocketID, params});
                    sessionRequestPushMutex.unlock();
                }
            }
        }
    }
}

/**
 * ClientInitializerThreads work on this method
 * not sessioned request are forwarded here
 * this creates a new session for the client and forwards the request to the Routes::service() method
 * send the response to the socket and closes it
 * also assign the session to a client session thread to carry out the client's(session) further requests
 */
void Server::processInitializerRequest(int tid){
    while(1){
        if(!clientInitializerThread[tid].requests.empty()){
            // get the next parsed request
            Request req = clientInitializerThread[tid].requests.front();
            clientInitializerThread[tid].requests.pop();

            // create new session
            newSessionIDMutex.lock();
            int newSessionID = Session::getNextSessionID();
            newSessionIDMutex.unlock();
            Session *newSession = new Session(newSessionID);

            // session management thread is informed
            sessionManagementThread.sessions[newSessionID] = bs(true, newSession);
            activeSessionPushMutex.lock();
            sessionManagementThread.activeSessionIDs.push(newSessionID);
            activeSessionPushMutex.unlock();

            // redirect the request to the routes and send response to the related socket
            string response = Routes::service(req.method, req.path, req.params, newSession);
            const int responseLen = response.size();
            write(req.responseSocketID , response.c_str() , responseLen);
            close(req.responseSocketID);

            // assign the further operation of this session to a client session thread that has the minimum number of clients
            int sessionThreadID = chooseSessionThread();
            sessionPushMutex.lock();
            clientSesssionThreads[sessionThreadID].clients.push(newSessionID);
            sessionPushMutex.unlock();
        }
    }
}

/**
 * ClientSessionThreads work on this method
 * this loops on the clients(sessions) and meets their requests
 * clients(sessions) are shared equally amoung client session threads
 * each thread loops on their clients(sessions)
 */
void Server::processSessionRequest(int tid){
    while(1){
        if(!clientSesssionThreads[tid].clients.empty()){
            // gets the next client(session)
            int sessionID = clientSesssionThreads[tid].clients.front();
            clientSesssionThreads[tid].clients.pop();
            bs session = sessionManagementThread.sessions[sessionID];

            // if the session is not expired process it and add the session to the end of the q again
            if(session.first){
                while(session.second->hasNextRequest()){
                    // get the next parsed request of the session and forward it to the Routes::service()
                    Request req = session.second->getNextRequest();
                    string response = Routes::service(req.method, req.path, req.params, session.second);

                    // writes the http response to the related socket, then close it
                    const int responseLen = response.size();
                    write(req.responseSocketID , response.c_str() , responseLen);
                    close(req.responseSocketID);
                }

                // add the session to end of the q again
                clientSesssionThreads[tid].clients.push(sessionID);
            }
        }
    }
}

/**
 * SessionManagementThread run on this method
 * loops on all sessions and check if they are used in last X times
 * if not used it expires the sessions
 */
void Server::cleanSessions(){
    const int expireTime = 60 * 10; // 10 minutes
    time_t cur;
    time(&cur);
    int counter = 0;

    while(1){
        // loops on the active sessions
        for( int i = 0 ; i < sessionManagementThread.activeSessionIDs.size() ; i++, counter++ ){
            if(counter == 500){
                time(&cur);
                counter = 0;
            }
            int sessionID = sessionManagementThread.activeSessionIDs.front();
            sessionManagementThread.activeSessionIDs.pop();
            bs &session = sessionManagementThread.sessions[sessionID];

            int diff = (int)difftime(cur, session.second->getLastActive());
            if(diff >= expireTime){ // expires the session if it not used for the last X time
                session.first = false;
                delete session.second;
                session.second = NULL;
            } else{ // if not expired add the active list again
                sessionManagementThread.activeSessionIDs.push(sessionID);
            }
        }
    }
}

/**
 * chooses a client session thread with the minimum number of clients(sessions)
 */
int Server::chooseSessionThread(){
    int mint = 0;
    for( int i = 1 ; i < CLIENT_SESSION_THREAD_COUNT ; i++ ){
        if(clientSesssionThreads[mint].clients.size() > clientSesssionThreads[i].clients.size()){
            mint = i;
        }
    }
    return mint;
}
