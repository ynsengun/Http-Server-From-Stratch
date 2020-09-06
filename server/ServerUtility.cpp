#include "ServerUtility.h"

/**
 * parses the http request as method, path and params
 */
void ServerUtil::parseRequest( char *buffer, string &method, string &path, int &sessionID, map<string, string> &params){
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
    if(method == "GET"){
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
    } else{
        for(; i + 1 < len ; i++ ){
            if(buffer[i] == '\n' && buffer[i+2] == '\n'){
                break;
            }
        }
        if(i + 1 != len){
            i += 2;
            while(i < len && (buffer[i] == '&' || buffer[i] == '\n')){
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
}
