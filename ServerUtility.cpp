#include "ServerUtility.h"

void ServerUtil::createIndexHTML( char *&index){
    ifstream ifs("index.html");
    string content( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>()) );
    const int indexLen = content.size();

    string responseStr = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    responseStr += to_string(indexLen);
    responseStr += "\n\n";
    responseStr += content;
    const int responseLen = responseStr.size();

    index = new char[responseLen];
    strcpy(index, responseStr.c_str());
}

void ServerUtil::createPostResponse( char *&postIndex, int socket_id){
    string responseStr = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    responseStr += "7";
    responseStr += "\n\n";
    responseStr += "posted " + to_string(socket_id);
    const int responseLen = responseStr.size();

    postIndex = new char[responseLen];
    strcpy(postIndex, responseStr.c_str());
}

void ServerUtil::getMethodAndParam( char *buffer, string &method, string &param){
    int len = strlen(buffer);
    int i;

    for( i = 0 ; i < len ; i++ ){
        if( buffer[i] == ' ')
            break;
        method += buffer[i];
    }

    if( buffer[i+1] == '/'){
        for( i+= 2 ; i < len ; i++ ){
            if(buffer[i] == ' ')
                break;
            param += buffer[i];
        }
    } else{
        param = "";
    }
}