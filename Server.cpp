#include "Server.h"
#include "ServerUtility.h"

// after complete init, the socket will be ready to accept connections
void Server::init(int port){
    // Creating socket file descriptor
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
    ServerUtil::createIndexHTML(indexHTML);
}

void Server::run(){
    int addrlen = sizeof(address);
    int new_socket;

    while(1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        printf("socket id: %d\n",new_socket);
        
        char buffer[30000] = {0};
        read( new_socket , buffer, 30000);

        string method = "";
        string param = "";

        ServerUtil::getMethodAndParam(buffer, method, param);

        cout<<method<<endl<<param<<endl;
        printf("%s\n",buffer );

        if( method == "GET" ){
            // create a thread maybe
            printf("get resp\n%s\n",indexHTML);
            write(new_socket , indexHTML , strlen(indexHTML));
            printf("------------------Hello message sent-------------------\n");
            close(new_socket);
        } else if( method == "PUT"){ // update
            cout<<"put"<<endl;
            // get updated value as well

            int id = atoi(param.c_str());

        } else if( method == "DELETE"){ // remove
            cout<<"delete"<<endl;

            int id = atoi(param.c_str());
            
        } else if( method == "POST"){ // insert
            char *postIndex;
            ServerUtil::createPostResponse(postIndex, new_socket);
            printf(" ---- resp ---- \n%s\n",postIndex);
            // write(new_socket , postIndex , strlen(postIndex));
            write(new_socket , indexHTML , strlen(indexHTML));
            printf("------------------Post message sent-------------------\n");
            // insert param
        }

    }
}

void Server::clear(){
    delete[] indexHTML;
}