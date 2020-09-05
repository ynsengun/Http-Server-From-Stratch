#include "ServerUtility.h"

void ServerUtil::parseRequest( char *buffer, string &method, string &path, int &sessionID, map<string, string> params){
    int len = strlen(buffer);
    int i;

    // parse method
    method = "";
    for( i = 0 ; i < len ; i++ ){
        if( buffer[i] == ' ')
            break;
        method += buffer[i];
    }

    // parse path
    path = "";
    if(buffer[i+1] == '/'){
        for( i++ ; i < len ; i++ ){
            if(buffer[i] == ' ' || buffer[i] == '?')
                break;
            path += buffer[i];
        }
    } else{
        path = "";
    }

    // parse query string parameters
    if(buffer[i] == '?'){
        while(buffer[i] == '?' || buffer[i] == '&'){
            string key = "";
            string value = "";
            for( i++ ; i < len ; i++ ){
                if(buffer[i] == '=')
                    break;
                key += buffer[i];
            }
            for( i++ ; i < len ; i++ ){
                if(buffer[i] == '&' || buffer[i] == ' ')
                    break;
                value += buffer[i];
            }
            if(key == "sessionID")
                sessionID = atoi(value.c_str());
            params[key] = value;
        }
    } else{
        sessionID = -1;
        params.clear();
    }
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
