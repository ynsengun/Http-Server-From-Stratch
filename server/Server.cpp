#include "Server.h"
#include "ServerUtility.h"

const static int REQUEST_BUFFER_SIZE = 1500;

// after complete init, the socket will be ready to accept connections
// so indexHTML response is created and all threads are initiaized
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

    indexHTML = NULL;
    ServerUtil::createIndexHTML(indexHTML); // TODO i think this is not the correct place for it

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

    // TODO session cleaner thread
    sessionCleanerThread.t = thread(&Server::cleanSessions, this);
}

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

            cout<<method<<endl<<path<<endl;
            cout<<"########################## start ###########################"<<endl;
            printf("%s\n",buffer );
            cout<<"########################## end ###########################"<<endl;

            if(sessionID == -1){
                // TODO forward to initializer thread
            } else{
                // TODO forward to session thread
            }

            // TODO move below
            if( method == "GET" ){
                // create a thread maybe
                printf("get resp\n%s\n",indexHTML);
                write(requestSocketID , indexHTML , strlen(indexHTML));
                printf("------------------Hello message sent-------------------\n");
                close(requestSocketID);
            } else if( method == "POST"){ // insert
                char *postIndex;
                ServerUtil::createPostResponse(postIndex, requestSocketID);
                printf(" ---- resp ---- \n%s\n",postIndex);
                // write(requestSocketID , postIndex , strlen(postIndex));
                write(requestSocketID , indexHTML , strlen(indexHTML));
                printf("------------------Post message sent-------------------\n");
                // insert param
            }
        }
    }
}

void Server::processSessionRequest(int tid){
    while(1){
        if(!clientSesssionThreads[tid].clients.empty()){
            int sessionID = clientSesssionThreads[tid].clients.front();
            clientSesssionThreads[tid].clients.pop();

            // TODO take the parsed request and handle them, probably forward them to router

            clientSesssionThreads[tid].clients.push(sessionID);
        }
    }
}

void Server::processInitializerRequest(int tid){
    while(1){
        if(!clientInitializerThread[tid].clientSocketIDs.empty()){
            // TODO give new session id and forward to the related router
        }
    }
}

void Server::cleanSessions(){
    while(1){
        if(!sessionCleanerThread.sessionIDs.empty()){
            // TODO look at the last activated time of the current session, is more than X time, terminate it
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

        workerThreads[nextWorker].requestSocketIDs.push(new_socket);
        nextWorker++;
        if(nextWorker >= WORKER_THREAD_COUNT){
            nextWorker -= WORKER_THREAD_COUNT;
        }
    }
}

void Server::clear(){
    delete[] indexHTML;
}