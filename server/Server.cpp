#include "Server.h"
#include "ServerUtility.h"
#include "../router/Router.h"

const static int REQUEST_BUFFER_SIZE = 1500;
int nextInitializer = 0;

// after complete init, the socket will be ready to accept connections
// static pages will be shaped and all threads are initiaized
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

    Routes::initializeStaticPages();
    initializeThreads();
}

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

mutex nextInitializerMutex, sessionRequestPushMutex;
void Server::parseRequest(int tid){
    while(1){
        if(!workerThreads[tid].requestSocketIDs.empty()){
            int requestSocketID = workerThreads[tid].requestSocketIDs.front();
            workerThreads[tid].requestSocketIDs.pop();

            char buffer[REQUEST_BUFFER_SIZE];
            read( requestSocketID , buffer, REQUEST_BUFFER_SIZE);

            string method = "";
            string path = "";
            int sessionID = -1;
            map<string, string> params;

            ServerUtil::parseRequest(buffer, method, path, sessionID, params);

            cout<<"\n########################## start ###########################"<<endl;
            cout<<"method path sessionID:   "<<method<<" "<<path<<" "<<sessionID<<endl;
            cout<<"item:  "<<params["item"]<<endl;
            printf("%s\n",buffer );
            cout<<"########################## end ###########################"<<endl;

            if(sessionID == -1 && path != "/favicon.ico"){ // TODO path control?
                cout<<"loaded on the initializer:   "<<nextInitializer<<endl;
                nextInitializerMutex.lock();
                clientInitializerThread[nextInitializer].clientSocketIDs.push(requestSocketID); // push request ?
                nextInitializer++;
                if(nextInitializer >= CLIENT_INITIALIZER_THREAD_COUNT){
                    nextInitializer -= CLIENT_INITIALIZER_THREAD_COUNT;
                }
                nextInitializerMutex.unlock();
            } else{
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

void Server::processSessionRequest(int tid){
    while(1){
        if(!clientSesssionThreads[tid].clients.empty()){
            int sessionID = clientSesssionThreads[tid].clients.front();
            clientSesssionThreads[tid].clients.pop();
            // cout<<"hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee "<<sessionID<<endl;

            bs session = sessionManagementThread.sessions[sessionID];

            if(session.first){
                while(session.second->hasNextRequest()){
                    Request req = session.second->getNextRequest();
                    string response;

                    if(req.method == "GET"){
                        response = Routes::httpGet(req.path, req.params, session.second);
                    } else if(req.method == "POST"){
                        response = Routes::httpPost(req.path, req.params, session.second);
                    } else if(req.method == "PUT"){
                        response = Routes::httpPut(req.path, req.params, session.second);
                    } else if(req.method == "DELETE"){
                        response = Routes::httpDelete(req.path, req.params, session.second);
                    }

                    cout<<"here is the response                 "<<response<<endl;
                    const int responseLen = response.size();
                    write(req.responseSocketID , response.c_str() , responseLen);
                    close(req.responseSocketID);
                }

                clientSesssionThreads[tid].clients.push(sessionID);
            }
        }
    }
}

int Server::chooseSessionThread(){
    int mint = 0;
    for( int i = 1 ; i < CLIENT_SESSION_THREAD_COUNT ; i++ ){
        if(clientSesssionThreads[mint].clients.size() > clientSesssionThreads[i].clients.size()){
            mint = i;
        }
    }
    return mint;
}

mutex newSessionIDMutex, activeSessionPushMutex, sessionPushMutex;
void Server::processInitializerRequest(int tid){
    while(1){
        if(!clientInitializerThread[tid].clientSocketIDs.empty()){
            int socketID = clientInitializerThread[tid].clientSocketIDs.front();
            clientInitializerThread[tid].clientSocketIDs.pop();

            newSessionIDMutex.lock();
            int newSessionID = Session::getNextSessionID();
            newSessionIDMutex.unlock();
            Session *newSession = new Session(newSessionID);
            sessionManagementThread.sessions[newSessionID] = bs(true, newSession);
            activeSessionPushMutex.lock();
            sessionManagementThread.activeSessionIDs.push(newSessionID);
            activeSessionPushMutex.unlock();

            map<string, string> params;
            string response = Routes::httpGet("/", params, newSession);
            const int responseLen = response.size();
            write(socketID , response.c_str() , responseLen);

            int sessionThreadID = chooseSessionThread();
            cout<<"chooosen thread:  "<<sessionThreadID<<endl;

            sessionPushMutex.lock();
            clientSesssionThreads[sessionThreadID].clients.push(newSessionID);
            sessionPushMutex.unlock();
            cout<<"size::::::::::::::::::::::::::::::::::: "<<clientSesssionThreads[sessionThreadID].clients.size()<<endl;
            cout<<sessionManagementThread.activeSessionIDs.size()<<endl;


            cout<<"New Session ID:   "<<newSessionID<<endl; // TODO delete
            cout<<"Thread ID:        "<<tid<<endl; // TODO delete


            close(socketID);
        }
    }
}

void Server::cleanSessions(){
    const int expireTime = 60 * 10; // 10 minutes
    time_t cur;
    time(&cur);
    int counter = 0;

    while(1){
        // cout<<"-------------------------------------------------------------"<<endl;
        for( int i = 0 ; i < sessionManagementThread.activeSessionIDs.size() ; i++, counter++ ){
            // cout<<"qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq      "<<sessionManagementThread.activeSessionIDs.size()<<endl;
            if(counter == 1000){
                time(&cur);
                counter = 0;
            }
            int sessionID = sessionManagementThread.activeSessionIDs.front();
            sessionManagementThread.activeSessionIDs.pop();
            bs &session = sessionManagementThread.sessions[sessionID];

            if(session.first){
                int diff = (int)difftime(cur, session.second->getLastActive());
                // cout<<"diffffffffffffffffffffffffffffffffff   "<<sessionID<<"  "<<diff<<endl;
                if(diff >= expireTime){
                    // cout<<"girdiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"<<endl;
                    session.first = false;
                    delete session.second;
                    session.second = NULL;
                } else{
                    sessionManagementThread.activeSessionIDs.push(sessionID);
                }
            }
        }
    }
}

void Server::run(){
    int addrlen = sizeof(address);
    int new_socket;
    int nextWorker = 0;

    while(1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n"); // TODO delete this line
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        cout<<"worker thread:   "<<nextWorker<<endl; // TODO delete

        workerThreads[nextWorker].requestSocketIDs.push(new_socket);
        nextWorker++;
        if(nextWorker >= WORKER_THREAD_COUNT){
            nextWorker -= WORKER_THREAD_COUNT;
        }
    }
}

void Server::clear(){
}